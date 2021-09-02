#define VERSION_4_18 1
#include "LuaMachine.h"
#include "LuaProviderPrivatePCH.h"
#include "Misc/Paths.h"
#if VERSION_4_18
#include "HAL/PlatformApplicationMisc.h"
#endif

FString FLuaMachine::LuaSearchPath;
FString FLuaMachine::LuaSearchCPath;

static void* LuaAlloc(void *Ud, void *Ptr, size_t OldSize, size_t NewSize)
{
	if (NewSize != 0)
	{
		return FMemory::Realloc(Ptr, NewSize);
	}
	else
	{
		FMemory::Free(Ptr);
		return NULL;
	}
}

static int32 LuaPanic(lua_State *lua_State)
{
	UE_LOG(LuaProviderPlugin, Error, TEXT("PANIC: unprotected error in call to Lua API(%s)"), UTF8_TO_TCHAR(lua_tostring(lua_State, -1)));
	return 0;
}

static lua_State* LuaNewState()
{
	lua_State* LuaState = lua_newstate(LuaAlloc, NULL);
	if (LuaState)
	{
		lua_atpanic(LuaState, &LuaPanic);
	}
	return LuaState;
}

FLuaMachine::FLuaMachine()
	:LuaState(nullptr)
{
}

FLuaMachine::~FLuaMachine()
{
	Close();
}

void FLuaMachine::Initialize()
{
	UE_LOG(LuaProviderPlugin, Log, TEXT("FLuaMachine::Initialize(or Re-initialize)"));
	// By default, go ahead and disable the screen saver. The user can still change this freely
#if VERSION_4_18
	FPlatformApplicationMisc::ControlScreensaver(FPlatformApplicationMisc::EScreenSaverAction::Disable);
#else
	FPlatformMisc::ControlScreensaver(FGenericPlatformMisc::Disable);
#endif
	Close();
	LuaState = LuaNewState();
	luaL_openlibs(LuaState);

	FString cpacExt(TEXT("?.so'"));
#if PLATFORM_WINDOWS
	LuaSearchPath = FPaths::ProjectPluginsDir();//FPaths::GamePluginsDir();
	LuaSearchCPath = LuaSearchPath;
	LuaSearchPath.Append(TEXT("LuaProvider/Source/LuaProvider/Lua/Script/"));
	LuaSearchCPath.Append(TEXT("LuaProvider/Content/Lib/Win64/Release/"));
	cpacExt = TEXT("?.dll'");
#endif
	//set path and cpath
	FString packagePath(TEXT("package.path = package.path .. ';"));
	packagePath.Append(LuaSearchPath);
	packagePath.Append(TEXT("?.lua'"));

	FString packageCPath(TEXT("package.cpath = package.cpath .. ';"));
	packageCPath.Append(LuaSearchCPath);
	packageCPath.Append(cpacExt);

	luaL_dostring(LuaState, TCHAR_TO_UTF8(*packagePath));
	luaL_dostring(LuaState, TCHAR_TO_UTF8(*packageCPath));
}

void FLuaMachine::Close()
{
	if (LuaState)
	{
		lua_close(LuaState);
		LuaState = NULL;
	}
}

bool FLuaMachine::LoadLuaString(const FString & code)
{
	FScopeLock Lock(&LuaVmCritical);
	check(LuaState);
	if (luaL_loadstring(LuaState, TCHAR_TO_UTF8(*code)) == 0)
	{
		UE_LOG(LuaProviderPlugin, Log, TEXT("load Lua string~>%s"), *code);
		// Set this pointer
		lua_pushlightuserdata(LuaState, this);
		lua_setglobal(LuaState, "this");
		return FLuaUtils::CallFunction(LuaState, NULL);
	}
	else
	{
		UE_LOG(LuaProviderPlugin, Warning, TEXT("Cannot load Lua script for %s: %s"), *code, UTF8_TO_TCHAR(lua_tostring(LuaState, -1)));
	}
	return false;
}

bool FLuaMachine::LoadLuaFile(const FString & file_name)
{
	FScopeLock Lock(&LuaVmCritical);
	check(LuaState);
#if PLATFORM_ANDROID
	//FString FileName = file_name;
	//AAsset * file = AAssetManager_open(AndroidThunkCpp_GetAssetManager(), TCHAR_TO_UTF8(*FileName), AASSET_MODE_UNKNOWN);
	//if (nullptr == file)
	//{
	//	UE_LOG(LuaProviderPlugin, Log, TEXT("Lua file not found.~>%s"), *FileName);
	//	return false;
	//}
	//off_t bufferSize = AAsset_getLength(file);
	//if (bufferSize > 0)
	//{
	//	UE_LOG(LuaProviderPlugin, Log, TEXT("Lua Code Length~>%d"), bufferSize);
	//	char *buffer = (char *)malloc(bufferSize + 1);
	//	buffer[bufferSize] = 0;
	//	int numBytesRead = AAsset_read(file, buffer, bufferSize);
	//	const FString code = FString(UTF8_TO_TCHAR(buffer));
	//	UE_LOG(LuaProviderPlugin, Log, TEXT("Read Lua Code ~>%s"), *code);
	//	LoadString(code);
	//	free(buffer);
	//}
	//AAsset_close(file);
	//return true;
#endif
	if (luaL_loadfile(LuaState, TCHAR_TO_UTF8(*(LuaSearchPath +file_name))) == 0)
	{
		UE_LOG(LuaProviderPlugin, Log, TEXT("load Lua script for %s"), *file_name);
		// Set this pointer
		lua_pushlightuserdata(LuaState, this);
		lua_setglobal(LuaState, "this");
		return FLuaUtils::CallFunction(LuaState, NULL);
	}
	else
	{
		UE_LOG(LuaProviderPlugin, Warning, TEXT("Cannot load Lua script for %s: %s"), *file_name, UTF8_TO_TCHAR(lua_tostring(LuaState, -1)));
	}

	return false;
}


bool FLuaMachine::CallFunction(const FString & func_name)
{
	FScopeLock Lock(&LuaVmCritical);
	check(LuaState);
	bool bSuccess = FLuaUtils::DoesFunctionExist(LuaState, TCHAR_TO_UTF8(*func_name));
	if (bSuccess)
	{
		bSuccess = FLuaUtils::CallFunction(LuaState, TCHAR_TO_UTF8(*func_name));
	}
	else
	{
		UE_LOG(LuaProviderPlugin, Warning, TEXT("Failed to call function '%s' "), *func_name);
	}
	return bSuccess;
}

float FLuaMachine::GetFloatProperty(const FString & name)
{
	FScopeLock Lock(&LuaVmCritical);
	check(LuaState);
	float Results = 0;
	lua_getglobal(LuaState, TCHAR_TO_UTF8(*name));

	if (lua_isnumber(LuaState, lua_gettop(LuaState)))
	{
		Results = (float)lua_tonumber(LuaState, -1); // -1 = top of stack
	}
	else
	{
		UE_LOG(LuaProviderPlugin, Warning, TEXT(" [LUA] Global '%s' is NaN. "), *name);
	}

	lua_pop(LuaState, 1);
	return Results;
}

FString FLuaMachine::GetStringProperty(const FString & name)
{
	FScopeLock Lock(&LuaVmCritical);
	check(LuaState);

	FString Results(TEXT(""));

	lua_getglobal(LuaState, TCHAR_TO_UTF8(*name));

	if (lua_isstring(LuaState, lua_gettop(LuaState)))
	{
		Results = UTF8_TO_TCHAR(lua_tostring(LuaState, -1));
	}
	else
	{
		UE_LOG(LuaProviderPlugin, Warning, TEXT(" [LUA] Global '%s' is not a String. "), *name);
	}

	lua_pop(LuaState, 1);
	return Results;
}

int FLuaMachine::GetIntProperty(const FString & name)
{
	return FMath::TruncToInt(GetFloatProperty(name));
}

bool FLuaMachine::GetBoolProperty(const FString & name)
{
	FScopeLock Lock(&LuaVmCritical);
	check(LuaState);

	bool Results = false;
	lua_getglobal(LuaState, TCHAR_TO_UTF8(*name));

	if (lua_isboolean(LuaState, lua_gettop(LuaState)))
	{
		Results = !!lua_toboolean(LuaState, -1); // -1 = top of stack
	}
	else
	{
		UE_LOG(LuaProviderPlugin, Warning, TEXT(" [LUA] Global '%s' is NaN. "), *name);
	}

	lua_pop(LuaState, 1);
	return Results;
}

void FLuaMachine::RegisterLibrary(const luaL_Reg Lib[], const ANSICHAR * LibName)
{
	FScopeLock Lock(&LuaVmCritical);
	check(LuaState);
	FLuaUtils::RegisterLibrary(LuaState, Lib, LibName);
}

FLuaMachine::FLuaBinaryBufferPtr FLuaMachine::GetBinaryData(const FString & data_name, const FString & data_len_name)
{
	FLuaBinaryBufferPtr buf;
	{
		FScopeLock Lock(&LuaVmCritical);
		check(LuaState);
		int32 len = 0;
		lua_getglobal(LuaState, TCHAR_TO_UTF8(*data_len_name));
		if (lua_isnumber(LuaState, lua_gettop(LuaState)))
		{
			len = (int)lua_tonumber(LuaState, -1); // -1 = top of stack
		}
		else
		{
			UE_LOG(LuaProviderPlugin, Warning, TEXT(" [LUA] Global '%s' is not a length of this binaray string. "), *data_len_name);
		}
		lua_pop(LuaState, 1);
		if (0 >= len)
			return buf;

		lua_getglobal(LuaState, TCHAR_TO_UTF8(*data_name));
		if (lua_isstring(LuaState, lua_gettop(LuaState)))
		{
			const char* bs_ = lua_tostring(LuaState, -1);
			buf = MakeShareable<FLuaBinaryBuffer>(new FLuaBinaryBuffer());
			buf->buf = (char*)malloc(sizeof(char)*len);
			buf->len = len;
			FMemory::Memcpy(buf->buf, bs_, sizeof(char)*len);
		}
		else
		{
			UE_LOG(LuaProviderPlugin, Warning, TEXT(" [LUA] Global '%s' is not a String of this binary string. "), *data_name);
		}

		lua_pop(LuaState, 1);
		//release the lock!
	}
	LoadLuaString(FString::Printf(TEXT("%s=nil %s=nil"), *data_name, *data_len_name));
	return buf;
}

bool FLuaMachine::SetBinaryData(const FString& name, const char * buf, const int len)
{
	FScopeLock Lock(&LuaVmCritical);
	check(LuaState);
	if (nullptr == buf || 0 == len)
		return false;
	lua_pushlstring(LuaState, buf, len);
	lua_setglobal(LuaState, TCHAR_TO_UTF8(*name));
	return true;
}

void FLuaMachine::FLuaUtils::RegisterLibrary(lua_State * LuaState, const luaL_Reg Lib[], const ANSICHAR * LibName)
{
	lua_getglobal(LuaState, LibName);
	if (lua_isnil(LuaState, -1))
	{
		UE_LOG(LuaProviderPlugin, Log, TEXT(" [LUA] Global '%s' is not found,try to create a new one as a table. "), ANSI_TO_TCHAR(LibName));
		lua_pop(LuaState, 1);
		lua_newtable(LuaState);
	}
	luaL_setfuncs(LuaState, Lib, 0);
	lua_setglobal(LuaState, LibName);
	UE_LOG(LuaProviderPlugin, Log, TEXT(" [LUA] Global '%s' created. "), ANSI_TO_TCHAR(LibName));
}

bool FLuaMachine::FLuaUtils::CallFunction(lua_State * LuaState, const ANSICHAR * FunctionName)
{
	if (FunctionName != NULL)
	{
		lua_getglobal(LuaState, FunctionName);
	}

	bool bResult = true;
	const int NumArgs = 0;
	const int NumResults = 0;
	if (lua_pcall(LuaState, NumArgs, NumResults, 0) != 0)
	{
		UE_LOG(LuaProviderPlugin, Warning, TEXT("Cannot call Lua function %s: %s"), ANSI_TO_TCHAR(FunctionName), ANSI_TO_TCHAR(lua_tostring(LuaState, -1)));
		bResult = false;
	}
	return bResult;
}

bool FLuaMachine::FLuaUtils::DoesFunctionExist(lua_State * LuaState, const ANSICHAR * FunctionName)
{
	bool bExists = true;
	lua_getglobal(LuaState, FunctionName);
	if (!lua_isfunction(LuaState, lua_gettop(LuaState)))
	{
		bExists = false;
	}
	lua_pop(LuaState, 1);
	return bExists;
}


