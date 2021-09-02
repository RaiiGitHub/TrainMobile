// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.
#include "LuaMachineActor.h"
#include "LuaProviderPrivatePCH.h"
#include "LuaProvider.h"

ALuaMachineActor::ALuaMachineActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
}

// Called every frame
void ALuaMachineActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
#if PLATFORM_ANDROID
	FString cmd;
	if (FJavaDataAccepter::JavaDataAccepterIns->GetJsCmdInQueue(cmd, true))
		NotifyJavascriptCommand(cmd); 
	int total = 0, download = 0;
	if (FJavaDataAccepter::JavaDataAccepterIns->GetDownloadLengthStatistics(total, download))
		NotifyDownloadStatistics(total, download);
#endif
}