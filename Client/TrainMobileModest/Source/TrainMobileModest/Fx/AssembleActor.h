// As Part of GuangZhou Training.

#pragma once

#include "GameFramework/Actor.h"
#include "GameUtilityBlueprintStruct.h"
#include "AssembleActor.generated.h"

UCLASS()
class TRAINMOBILEMODEST_API AAssembleActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAssembleActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
public:
	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	bool SetupAssembleMeshWithSockets(UStaticMeshComponent* mesh_component,const TArray<FGameKeyValue>& sokcet_mesh_src);

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	TArray<AActor*> GetAssembleChildActors(UStaticMeshComponent* mesh_component);
};
