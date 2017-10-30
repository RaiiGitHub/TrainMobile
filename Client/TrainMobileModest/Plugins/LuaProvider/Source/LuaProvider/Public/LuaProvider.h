// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "ModuleManager.h"
#include "SprotoPacker.h"
#include "JavaDataAccepter.h"

class FLuaProviderModule : public IModuleInterface
{
public:
	friend LUAPROVIDER_API TSharedPtr<FSprotoPacker> GetSprotoPacker();
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
private:
	static TSharedPtr<FSprotoPacker>  SprotoPacker;
	/** Handle to the test dll we will load */
	void*	LuaLibraryHandle = nullptr;

};

TSharedPtr<FSprotoPacker> LUAPROVIDER_API GetSprotoPacker();