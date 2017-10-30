// As Part of GuangZhou Training.

#pragma once

#include "GameFramework/Actor.h"
#include "SunlightSimulator.h"
#include "SunlightController.generated.h"

UCLASS()
class TRAINMOBILEMODEST_API ASunlightController 
	: public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASunlightController();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;
private:
	SunlightSimulator sunlightSimulator;
public:
	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	void SetSimulateSpeed(float percentPerSecond);

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	void SetSimulateCount(int count);//-1 for infinited.

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	void SetSimulateSunHeightPercent(float percent);//usually fix setting

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	void BeginSimulate();

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	void PauseSimulate();

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	void ResetSimulate();
public:
	//sun light.
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "DispatchNotify")
	bool SetSunHeight(float sunHeight);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "DispatchNotify")
	bool SetHorizonFallOff(float horizonFalloff);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "DispatchNotify")
	bool SetZenithColor(FLinearColor color);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "DispatchNotify")
	bool SetCloudColor(FLinearColor color);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "DispatchNotify")
	bool SetOverallColor(FLinearColor color);

public:
	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	void SetSystemSunHeightPercent(float percent);

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	void SetSunHeightPercentRate(float percentRate);
};
