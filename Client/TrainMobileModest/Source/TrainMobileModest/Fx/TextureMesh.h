// As Part of GuangZhou Training.

#pragma once

#include "GameFramework/Actor.h"
#include "TextureMesh.generated.h"

UCLASS()
class TRAINMOBILEMODEST_API ATextureMesh : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATextureMesh();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

public:
	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	bool ReShapeHorizonLineMesh(FVector start, FVector end, AStaticMeshActor* mesh, float offsetHeight,float uRate,const FString& collectionPath,FName scalarParamName);

	
};
