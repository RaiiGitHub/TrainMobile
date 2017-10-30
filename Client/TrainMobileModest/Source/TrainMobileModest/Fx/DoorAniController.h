// As Part of GuangZhou Training.

#pragma once

#include "GameFramework/Actor.h"
#include "DoorAniController.generated.h"

UENUM(BlueprintType)
enum EAniState
{
	AniNotSet = 0,
	AniOpening = 1,
	AniClosing = 2,
	AniOpenPause = 3,
	AniClosePause = 4,
	AniOpenDone = 5,
	AniCloseDone = 6
};

UCLASS()
class TRAINMOBILEMODEST_API ADoorAniController : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADoorAniController();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;
public:
	/**Door Open percent,0~1*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = DoorAnimation)
	float OpenPercent;
	/**Door Open speed rate,percentage/s*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = DoorAnimation)
	float PercentagePerSecond;
public:
	// open animation
	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	bool OpenDoor();
	// close animation
	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	bool CloseDoor();
	// pause animation
	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	void Pause();
	// resume animation
	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	void Resume();
	// reset
	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	void Reset();
	// get status
	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	EAniState GetCurrentState();
private:
	EAniState DoorAniState;
};
