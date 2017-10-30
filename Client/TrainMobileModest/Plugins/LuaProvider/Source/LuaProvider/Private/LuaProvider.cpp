// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.
#include "LuaProviderPrivatePCH.h"
#include "LuaProvider.h"
#include "ModuleManager.h"
#include "IPluginManager.h"

IMPLEMENT_MODULE(FLuaProviderModule, LuaProvider)

DEFINE_LOG_CATEGORY(LuaProviderPlugin);

#define LOCTEXT_NAMESPACE "FLuaProviderModule"

TSharedPtr<FSprotoPacker> FLuaProviderModule::SprotoPacker;

void FLuaProviderModule::StartupModule()
{
    // This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	UE_LOG(LuaProviderPlugin, Log, TEXT("FLuaProviderModule::StartupModule"));
#if PLATFORM_WINDOWS
	FString BaseDir = IPluginManager::Get().FindPlugin("LuaProvider")->GetBaseDir();
	// Add on the relative location of the third party dll and load it
	FString LibraryPath;
	LibraryPath = FPaths::Combine(*BaseDir, TEXT("Content/Lib/Win64/Release/lua53.dll"));
	LuaLibraryHandle = !LibraryPath.IsEmpty() ? FPlatformProcess::GetDllHandle(*LibraryPath) : nullptr;
	if (nullptr == LuaLibraryHandle)
		FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("LuaLibraryError", "Failed to load lua library"));
#endif
}

void FLuaProviderModule::ShutdownModule()
{
    // This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
#if PLATFORM_WINDOWS
	FPlatformProcess::FreeDllHandle(LuaLibraryHandle);
	LuaLibraryHandle = nullptr;
#endif
}

TSharedPtr<FSprotoPacker> GetSprotoPacker()
{
	if (nullptr == FLuaProviderModule::SprotoPacker.Get())
	{
		FLuaProviderModule::SprotoPacker = MakeShareable(new FSprotoPacker);
		FLuaProviderModule::SprotoPacker->Initialize();
	}	
	return FLuaProviderModule::SprotoPacker;
}
