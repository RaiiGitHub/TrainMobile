// As Part of GuangZhou Training.

#pragma once

#include "GameFramework/Actor.h"
#include "TextureFinder.generated.h"

UCLASS()
class TRAINMOBILEMODEST_API ATextureFinder : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATextureFinder();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

public:
	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
		UTexture2D* LoadTexture(const FString& texPath);
	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
		FString GetMethodSetName() const;
	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
		void UnloadTexture();
protected:
	TSet<UTexture2D*> LoadedTextures_;
	FString MethodSetName_;
};
