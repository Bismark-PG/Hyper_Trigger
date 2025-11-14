/*==============================================================================

	Make Debug Ostream [debug_ostream.h]

	Author : Choi HyungJoon

==============================================================================*/
#ifndef DEBUG_OSTREAM_H
#define DEBUG_OSTREAM_H

#include <Windows.h>
#include <sstream>

using namespace std;

namespace Debug
{
	class Debug_Buf : public basic_stringbuf<char, char_traits<char>>
	{
	public:
		virtual ~Debug_Buf()
		{
			sync();
		}
	protected:
		int sync()
		{
			OutputDebugStringA(str().c_str());
			str(basic_string<char>());
			return 0;
		}
	};

	class debug_ostream : public basic_ostream<char, char_traits<char>>
	{
	public:
		debug_ostream()
			: basic_ostream<char, char_traits<char>>(new Debug_Buf()) {}
		~debug_ostream() { delete rdbuf(); }
	};

	extern debug_ostream D_Out;
}
#endif // BASIC_DEBUG_OSTREAM_H