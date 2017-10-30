// As Part of GuangZhou Training.

#include "TrainMobileModest.h"
#include "ActorFinder.h"


// Sets default values
AActorFinder::AActorFinder()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void AActorFinder::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AActorFinder::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

AActor * AActorFinder::FindActor(const FString & ActorName)
{
	AActor* actor = FindObject<AActor>(ANY_PACKAGE, *ActorName);
	return actor;
}

