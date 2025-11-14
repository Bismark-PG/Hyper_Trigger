/*==============================================================================

	Manage Model Logic [Model.h]

	Author : Choi HyungJoon

==============================================================================*/
#ifndef MODEL_H
#define MODEL_H
#include <unordered_map>
#include <d3d11.h>
#include <DirectXMath.h>
#include "cimport.h"
#include "scene.h"
#include "postprocess.h"
#include "matrix4x4.h"
#pragma comment (lib, "assimp-vc143-mt.lib")
#include "Collision.h"

struct MODEL
{
	const aiScene* AiScene = nullptr;

	ID3D11Buffer** VertexBuffer = nullptr;
	ID3D11Buffer** IndexBuffer = nullptr;

	std::unordered_map<std::string, ID3D11ShaderResourceView*> Texture;

	AABB Local_AABB = {};
};

MODEL* ModelLoad(const char* FileName, float scale = 1.0f, bool bBlender = false);
void ModelRelease(MODEL* model);

void ModelDraw(MODEL* model, const DirectX::XMMATRIX& mtxWorld);

AABB Model_Get_AABB(MODEL* model, const  DirectX::XMFLOAT3& position);
#endif // MODEL_H