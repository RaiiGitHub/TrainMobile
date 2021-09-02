// As Part of GuangZhou Training.

#include "DoorAniController.h"
#include "TrainMobileModest.h"


// Sets default values
ADoorAniController::ADoorAniController()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	DoorAniState = AniNotSet;
	PercentagePerSecond = 1.0f;
}

// Called when the game starts or when spawned
void ADoorAniController::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ADoorAniController::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
	if (AniOpenPause == DoorAniState || AniClosePause == DoorAniState )
		return;
	else if (AniOpening == DoorAniState)
	{
		float change_percent = PercentagePerSecond * DeltaTime;
		OpenPercent += change_percent;
		if (OpenPercent >= 1.f)
		{
			OpenPercent = 1.f;
			DoorAniState = AniOpenDone;
		}
	}
	else if (AniClosing == DoorAniState)
	{
		float change_percent = PercentagePerSecond * DeltaTime;
		OpenPercent -= change_percent;
		if (OpenPercent <= 0.f)
		{
			OpenPercent = 0.f;
			DoorAniState = AniCloseDone;
		}
	}
}

bool ADoorAniController::OpenDoor()
{
	if (AniOpenDone == DoorAniState)
	{
		DoorAniState = AniOpening;
		return false;
	}
	else if (AniOpening == DoorAniState)
	{
		return false;//already opening
	}
	DoorAniState = AniOpening;
	return true;
}

bool ADoorAniController::CloseDoor()
{
	if (AniCloseDone == DoorAniState)
	{
		DoorAniState = AniClosing;
		return false;
	}
	else if (AniClosing == DoorAniState)
	{
		return false;//already closing
	}
	DoorAniState = AniClosing;
	return true;
}

void ADoorAniController::Pause()
{
	if (AniOpening == DoorAniState)
		DoorAniState = AniOpenPause;
	else if (AniClosing == DoorAniState)
		DoorAniState = AniClosePause;
}

void ADoorAniController::Resume()
{
	if (AniOpenPause == DoorAniState)
		DoorAniState = AniOpening;
	else if (AniClosePause == DoorAniState)
		DoorAniState = AniClosing;
}

void ADoorAniController::Reset()
{
	DoorAniState = AniNotSet;
	OpenPercent = 0.f;
}

EAniState ADoorAniController::GetCurrentState()
{
	return DoorAniState;
}

