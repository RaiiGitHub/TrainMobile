// As Part of GuangZhou Training.

#pragma once

#include "GameFramework/Actor.h"
#include "ActorFinder.generated.h"

UCLASS()
class TRAINMOBILEMODEST_API AActorFinder : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AActorFinder();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

public:
	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
		AActor* FindActor(const FString& ActorName);
	
};
