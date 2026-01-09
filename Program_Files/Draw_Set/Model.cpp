/*==============================================================================

	Manage Model Logic [Model.cpp]

	Author : Choi HyungJoon

==============================================================================*/
#include <assert.h>
#include "direct3d.h"
#include "Texture_Manager.h"   
#include "Model.h"
#include <DirectXMath.h>
#include "WICTextureLoader11.h"
#include "Shader_Manager.h"    
#include <string>
#include "debug_ostream.h"

using namespace DirectX;

//3D頂点構造体
struct Vertex3D
{
	XMFLOAT3 position; // 頂点座標
	XMFLOAT3 normal;   //法線
	XMFLOAT4 color;    // 色
	XMFLOAT2 texcoord; // UV
};

static int g_TextureWhite = -1; // "TextSample" ID

MODEL* ModelLoad(const char* FileName, float scale, bool bBlender)
{
	if (g_TextureWhite == -1)
	{
		g_TextureWhite = Texture_Manager::GetInstance()->GetID("TextSample");
	}

	MODEL* model = new MODEL;

	model->AiScene = aiImportFile(FileName, aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_ConvertToLeftHanded);

	if (model->AiScene == nullptr)
	{
		Debug::D_Out << "Error: aiImportFile failed to load file: " << FileName << std::endl;
		delete model;
		return nullptr;
	}

	model->Local_AABB.Min = { FLT_MAX, FLT_MAX, FLT_MAX };
	model->Local_AABB.Max = { -FLT_MAX, -FLT_MAX, -FLT_MAX };

	model->VertexBuffer = new ID3D11Buffer * [model->AiScene->mNumMeshes]();
	model->IndexBuffer = new ID3D11Buffer * [model->AiScene->mNumMeshes]();

	// ===================================================================
	// Loop for Index Mesh
	// ===================================================================
	for (unsigned int m = 0; m < model->AiScene->mNumMeshes; m++)
	{
		aiMesh* mesh = model->AiScene->mMeshes[m];

		aiMaterial* aimaterial = model->AiScene->mMaterials[mesh->mMaterialIndex];

		aiString textureName;
		aimaterial->GetTexture(aiTextureType_DIFFUSE, 0, &textureName);

		// Get Diffuse Color
		XMFLOAT4 meshColor = { 1.0f, 1.0f, 1.0f, 1.0f };
		aiColor3D diffuse(1.0f, 1.0f, 1.0f);

		if (aimaterial->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse) == AI_SUCCESS)
		{
			// [Safety Fix] If Diffuse is Black(0,0,0), Force White
			if (diffuse.r <= 0.01f && diffuse.g <= 0.01f && diffuse.b <= 0.01f)
			{
				meshColor = { 1.0f, 1.0f, 1.0f, 1.0f };
			}
			else
			{
				meshColor = { diffuse.r, diffuse.g, diffuse.b, 1.0f };
			}
		}

		// 頂点バッファ生成
		Vertex3D* vertex = new Vertex3D[mesh->mNumVertices];

		for (unsigned int v = 0; v < mesh->mNumVertices; v++)
		{
			// POS
			if (bBlender) {
				vertex[v].position = {
					mesh->mVertices[v].x * scale,
					mesh->mVertices[v].z * scale, // Y and Z
					mesh->mVertices[v].y * scale, //
				};
			}
			else {
				vertex[v].position = {
					mesh->mVertices[v].x * scale,
					mesh->mVertices[v].y * scale,
					mesh->mVertices[v].z * scale,
				};
			}

			// 法線
			if (mesh->HasNormals()) {
				if (bBlender) {
					vertex[v].normal = {
						mesh->mNormals[v].x,
						mesh->mNormals[v].z, // Y and Z
						mesh->mNormals[v].y, // 
					};
				}
				else {
					vertex[v].normal = {
						mesh->mNormals[v].x,
						mesh->mNormals[v].y,
						mesh->mNormals[v].z,
					};
				}
			}
			else {
				vertex[v].normal = { 0.0f,1.0f,0.0f };
			}


			// 色
			if (mesh->HasVertexColors(0))
			{
				vertex[v].color = {
					mesh->mColors[0][v].r,
					mesh->mColors[0][v].g,
					mesh->mColors[0][v].b,
					mesh->mColors[0][v].a,
				};
			}
			else
			{
				vertex[v].color = meshColor;
			}

			// UV
			if (mesh->HasTextureCoords(0)) {
				vertex[v].texcoord = {
					mesh->mTextureCoords[0][v].x,
					mesh->mTextureCoords[0][v].y
				};
			}
			else {
				vertex[v].texcoord = { 0.0f,0.0f };
			}

			// Make AABB
			model->Local_AABB.Min.x = std::min(model->Local_AABB.Min.x, vertex[v].position.x);
			model->Local_AABB.Min.y = std::min(model->Local_AABB.Min.y, vertex[v].position.y);
			model->Local_AABB.Min.z = std::min(model->Local_AABB.Min.z, vertex[v].position.z);

			model->Local_AABB.Max.x = std::max(model->Local_AABB.Max.x, vertex[v].position.x);
			model->Local_AABB.Max.y = std::max(model->Local_AABB.Max.y, vertex[v].position.y);
			model->Local_AABB.Max.z = std::max(model->Local_AABB.Max.z, vertex[v].position.z);
		}

		// 頂点バッファ生成
		D3D11_BUFFER_DESC buffer_desc{};
		buffer_desc.ByteWidth = sizeof(Vertex3D) * mesh->mNumVertices;
		buffer_desc.Usage = D3D11_USAGE_DEFAULT;
		buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA subresource_data{};
		subresource_data.pSysMem = vertex;

		Direct3D_GetDevice()->CreateBuffer(&buffer_desc, &subresource_data, &model->VertexBuffer[m]);

		delete[] vertex;

		// インデックスバッファ生成
		unsigned int* index = new unsigned int[mesh->mNumFaces * 3];

		for (unsigned int f = 0; f < mesh->mNumFaces; f++)
		{
			index[f * 3 + 0] = mesh->mFaces[f].mIndices[0];
			index[f * 3 + 1] = mesh->mFaces[f].mIndices[1];
			index[f * 3 + 2] = mesh->mFaces[f].mIndices[2];
		}

		buffer_desc.ByteWidth = sizeof(unsigned int) * mesh->mNumFaces * 3;
		buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		subresource_data.pSysMem = index;
		Direct3D_GetDevice()->CreateBuffer(&buffer_desc, &subresource_data, &model->IndexBuffer[m]);

		delete[] index;
	}
	// ===================================================================
	// loop done
	// ===================================================================


	// ===================================================================
	// 2. Texture Load
	// ===================================================================
	for (unsigned int i = 0; i < model->AiScene->mNumTextures; i++)
	{
		aiTexture* AI_Tex = model->AiScene->mTextures[i];

		ID3D11ShaderResourceView* texture;
		ID3D11Resource* resource;

		CreateWICTextureFromMemory(
			Direct3D_GetDevice(),
			Direct3D_GetContext(),
			(const uint8_t*)AI_Tex->pcData,
			((size_t)AI_Tex->mWidth == 0) ? (size_t)AI_Tex->mHeight : (size_t)AI_Tex->mWidth,
			&resource,
			&texture);

		assert(texture);

		resource->Release();
		model->Texture[AI_Tex->mFilename.data] = texture;
	}

	const std::string modelPath(FileName);
	size_t pos = modelPath.find_last_of("/\\");
	std::string directory = (pos != std::string::npos) ? modelPath.substr(0, pos + 1) : "";

	for (unsigned int i = 0; i < model->AiScene->mNumMaterials; i++)
	{
		aiString texPath;
		aiMaterial* aimaterial = model->AiScene->mMaterials[i];

		if (aimaterial->GetTexture(aiTextureType_DIFFUSE, 0, &texPath) == AI_SUCCESS)
		{
			if (model->Texture.find(texPath.C_Str()) == model->Texture.end())
			{
				ID3D11ShaderResourceView* texture = nullptr;
				ID3D11Resource* resource = nullptr;

				std::string sTexPath = texPath.C_Str();
				size_t namePos = sTexPath.find_last_of("/\\");
				if (namePos != std::string::npos)
				{
					sTexPath = sTexPath.substr(namePos + 1);
				}

				// Combine Directory + Filename
				std::string texfilename = directory + sTexPath;
				std::replace(texfilename.begin(), texfilename.end(), '\\', '/');
				Debug::D_Out << "Loading Texture: " << texfilename << std::endl;

				// WideChar Conversion
				int len = MultiByteToWideChar(CP_UTF8, 0, texfilename.c_str(), -1, nullptr, 0);
				wchar_t* pWideFilename = new wchar_t[len];
				MultiByteToWideChar(CP_UTF8, 0, texfilename.c_str(), -1, pWideFilename, len);

				HRESULT hr = CreateWICTextureFromFile(
					Direct3D_GetDevice(),
					pWideFilename,
					&resource,
					&texture);

				delete[] pWideFilename;

				if (SUCCEEDED(hr))
				{
					if (resource) resource->Release();
					model->Texture[texPath.C_Str()] = texture;
				}
				else
				{
					Debug::D_Out << "Failed to load external texture: " << texfilename.c_str() << std::endl;
					// Keep nullptr, fallback handled in Draw
					model->Texture[texPath.C_Str()] = nullptr;
				}
			}
		}
	}

	return model;
}

void ModelRelease(MODEL* model)
{
	if (!model) return;

	if (model->AiScene)
	{
		for (unsigned int m = 0; m < model->AiScene->mNumMeshes; m++)
		{
			SAFE_RELEASE(model->VertexBuffer[m]);
			SAFE_RELEASE(model->IndexBuffer[m]);
		}
	}

	delete[] model->VertexBuffer;
	delete[] model->IndexBuffer;

	if (model->AiScene)
	{
		aiReleaseImport(model->AiScene);
		model->AiScene = nullptr;
	}

	delete model;
}

void ModelDraw(MODEL* model, const DirectX::XMMATRIX& mtxWorld)
{
	if (!model) return;

	float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	Direct3D_GetContext()->OMSetBlendState(nullptr, blendFactor, 0xffffffff);

	Shader_Manager::GetInstance()->Begin3D();
	Shader_Manager::GetInstance()->SetWorldMatrix3D(mtxWorld);

	// プリミティブトポロジ設定
	Direct3D_GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	for (unsigned int m = 0; m < model->AiScene->mNumMeshes; m++)
	{
		aiMaterial* aimaterial = model->AiScene->mMaterials[model->AiScene->mMeshes[m]->mMaterialIndex];

		// Get Diffuse Color And Opacity
		aiColor4D diffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
		if (aimaterial->Get(AI_MATKEY_COLOR_DIFFUSE, diffuseColor) == AI_SUCCESS)
		{
			if (diffuseColor.r < 0.01f && diffuseColor.g < 0.01f && diffuseColor.b < 0.01f)
			{
				diffuseColor = { 1.0f, 1.0f, 1.0f, 1.0f };
			}
		}

		float opacity = 1.0f;
		aimaterial->Get(AI_MATKEY_OPACITY, opacity);
		diffuseColor.a *= opacity;


		// Get Texture
		aiString texturePath;
		aimaterial->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath);

		ID3D11ShaderResourceView* pTexture = nullptr;
		bool textureFound = false;

		if (texturePath.length != 0)
		{
			auto it = model->Texture.find(texturePath.C_Str());
			if (it != model->Texture.end() && it->second != nullptr) // If Find Texture And Load Success
			{
				pTexture = it->second;
				textureFound = true;
			}
		}

		if (!textureFound) // If Cant`t Find Texture And Load Failed
		{
			pTexture = Texture_Manager::GetInstance()->Get_Shader_Resource_View(g_TextureWhite);

			// If No Texture DiffuseColor = White
			diffuseColor.r = 1.0f;
			diffuseColor.g = 1.0f;
			diffuseColor.b = 1.0f;
			diffuseColor.a = 1.0f;
		}

		// Input Diffuse Color And Opacity To Shader
		Shader_Manager::GetInstance()->SetDiffuseColor(XMFLOAT4(diffuseColor.r, diffuseColor.g, diffuseColor.b, diffuseColor.a));
		Shader_Manager::GetInstance()->SetTexture3D(pTexture);

		// 頂点バッファを描画パイプラインに設定
		UINT stride = sizeof(Vertex3D);
		UINT offset = 0;
		Direct3D_GetContext()->IASetVertexBuffers(0, 1, &model->VertexBuffer[m], &stride, &offset);

		// インデックスバッファを描画パイプラインに設定
		Direct3D_GetContext()->IASetIndexBuffer(model->IndexBuffer[m], DXGI_FORMAT_R32_UINT, 0);

		// 描画
		Direct3D_GetContext()->DrawIndexed(model->AiScene->mMeshes[m]->mNumFaces * 3, 0, 0);
	}
}

AABB Model_Get_AABB(MODEL* model, const DirectX::XMFLOAT3& position)
{
	return
	{
		{position.x + model->Local_AABB.Min.x, position.y + model->Local_AABB.Min.y, position.z + model->Local_AABB.Min.z},
		{position.x + model->Local_AABB.Max.x, position.y + model->Local_AABB.Max.y, position.z + model->Local_AABB.Max.z}
	};
}
