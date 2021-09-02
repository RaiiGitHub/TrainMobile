// As Part of GuangZhou Training.

#include "SunlightController.h"
#include "TrainMobileModest.h"


// Sets default values
ASunlightController::ASunlightController()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	sunlightSimulator.SetSunlightController(this);
}

// Called when the game starts or when spawned
void ASunlightController::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASunlightController::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
	sunlightSimulator.Tick(DeltaTime);

}

void ASunlightController::SetSimulateSpeed(float percentPerSecond)
{
	sunlightSimulator.SetSimulateSpeed(percentPerSecond);
}

void ASunlightController::SetSimulateCount(int count)
{
	sunlightSimulator.SetSimulateCount(count);
}

void ASunlightController::SetSimulateSunHeightPercent(float percent)
{
	sunlightSimulator.SetSimulateSunHeightPercent(percent);
}

void ASunlightController::BeginSimulate()
{
	sunlightSimulator.Begin();
}

void ASunlightController::PauseSimulate()
{
	sunlightSimulator.Pause();
}

void ASunlightController::ResetSimulate()
{
	sunlightSimulator.Reset();
}

void ASunlightController::SetSystemSunHeightPercent(float percent)
{
	sunlightSimulator.SetSystemSunHeightPercent(percent);
}

void ASunlightController::SetSunHeightPercentRate(float percentRate)
{
	sunlightSimulator.SetSimulateSpeed(percentRate);
}

