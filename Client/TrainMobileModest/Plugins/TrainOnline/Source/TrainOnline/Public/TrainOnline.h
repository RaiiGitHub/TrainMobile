// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "ModuleManager.h"
#include "LobbyProducer.h"
#include "SocketProducer.h"
#include "UserProducer.h"
#include "LobbyStateMachine.h"
#include "NotificationTeleporter.h"

class FTrainOnlineModule : public IModuleInterface
{
	friend TRAINONLINE_API TSharedPtr<FLobbyProducer> GetLobbyProducer();
	friend TRAINONLINE_API TSharedPtr<FSocketProducer> GetSocketProducer();
	friend TRAINONLINE_API TSharedPtr<FUserProducer> GetUserProducer();
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
private:
	static TSharedPtr<FLobbyProducer>  LobbyProducer;
	static TSharedPtr<FSocketProducer> SocketProducer;
	static TSharedPtr<FUserProducer>   UserProducer;
};

TSharedPtr<FLobbyProducer> TRAINONLINE_API GetLobbyProducer();
TSharedPtr<FSocketProducer>TRAINONLINE_API GetSocketProducer();
TSharedPtr<FUserProducer>  TRAINONLINE_API GetUserProducer();
