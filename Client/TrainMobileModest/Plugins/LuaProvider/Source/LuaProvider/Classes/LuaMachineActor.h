// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once
#include "LuaMachineActor.generated.h"
UCLASS()
class LUAPROVIDER_API ALuaMachineActor : public AActor
{
	GENERATED_UCLASS_BODY()
public:
	// Called every frame
	virtual void Tick(float DeltaSeconds) override;
public:
	//notify command from js side.
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "DispatchNotify")
	void NotifyJavascriptCommand(const FString& cmd);
	//notify download statistics
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "DispatchNotify")
	void NotifyDownloadStatistics(int total,int download);
};
