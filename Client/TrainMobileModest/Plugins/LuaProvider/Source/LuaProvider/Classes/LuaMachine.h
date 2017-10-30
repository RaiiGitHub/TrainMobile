#pragma once

extern "C"
{
#define LUA_COMPAT_APIINTCASTS 1
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}

class LUAPROVIDER_API FLuaMachine
{
public:
	FLuaMachine();
	~FLuaMachine();
private:
	struct FLuaUtils
	{
		static void RegisterLibrary(lua_State* LuaState, const luaL_Reg Lib[], const ANSICHAR* LibName);
		static bool CallFunction(lua_State* LuaState, const ANSICHAR* FunctionName);
		static bool DoesFunctionExist(lua_State* LuaState, const ANSICHAR* FunctionName);
	};
public:
	struct FLuaBinaryBuffer
	{
		FLuaBinaryBuffer() :buf(nullptr), len(0) {}
		~FLuaBinaryBuffer() {
			if (buf)
				free(buf);
		}
		char* buf;
		int32 len;
	};
	typedef TSharedPtr<FLuaBinaryBuffer> FLuaBinaryBufferPtr;
public:
	void Initialize();
	void Close();
	bool LoadLuaString(const FString& code);
	bool LoadLuaFile(const FString& file_name);
	bool CallFunction(const FString& func_name);
	float GetFloatProperty(const FString& name);
	FString GetStringProperty(const FString& name);
	int GetIntProperty(const FString& name);
	bool GetBoolProperty(const FString& name);
	void RegisterLibrary(const luaL_Reg Lib[], const ANSICHAR* LibName);
	FLuaBinaryBufferPtr GetBinaryData(const FString & data_name, const FString & data_len_name);
	bool SetBinaryData(const FString& name,const char* buf, const int len);
private:
	FCriticalSection LuaVmCritical;
private:
	lua_State* LuaState;
public:
	static FString LuaSearchPath;
	static FString LuaSearchCPath;
};

