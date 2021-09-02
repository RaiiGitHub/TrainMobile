// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "TrainOnline.h"
#include "TrainOnlinePrivatePCH.h"

IMPLEMENT_MODULE(FTrainOnlineModule, TrainOnline)

DEFINE_LOG_CATEGORY(TrainOnlinePlugin);

void FTrainOnlineModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	UE_LOG(TrainOnlinePlugin, Log, TEXT("FTrainOnlineModule::StartupModule"));
}

void FTrainOnlineModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

TSharedPtr<FLobbyProducer> FTrainOnlineModule::LobbyProducer = nullptr;
TSharedPtr<FSocketProducer> FTrainOnlineModule::SocketProducer = nullptr;
TSharedPtr<FUserProducer> FTrainOnlineModule::UserProducer = nullptr;

TSharedPtr<FLobbyProducer> GetLobbyProducer()
{
	if (nullptr == FTrainOnlineModule::LobbyProducer.Get())
		FTrainOnlineModule::LobbyProducer = MakeShareable(new FLobbyProducer);
	return FTrainOnlineModule::LobbyProducer;
}

TSharedPtr<FSocketProducer> GetSocketProducer()
{
	if (nullptr == FTrainOnlineModule::SocketProducer.Get())
		FTrainOnlineModule::SocketProducer = MakeShareable(new FSocketProducer);
	return FTrainOnlineModule::SocketProducer;
}

TSharedPtr<FUserProducer> GetUserProducer()
{
	if (nullptr == FTrainOnlineModule::UserProducer.Get())
		FTrainOnlineModule::UserProducer = MakeShareable(new FUserProducer);
	return FTrainOnlineModule::UserProducer;
}
