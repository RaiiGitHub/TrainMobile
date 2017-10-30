// As Part of GuangZhou Training.

#pragma once

#include "GameFramework/Actor.h"
#include "DynamicMaterial.h"
#include "DynamicInstanceMaterialBuilder.generated.h"

//no addition effect, just to build the instance of the claim material.

UCLASS()
class TRAINMOBILEMODEST_API ADynamicInstanceMaterialBuilder : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADynamicInstanceMaterialBuilder();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
private:
	DynamicMaterial Dm;
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	bool Begin(const FString& mesh_name, UMeshComponent* mesh, FName tex_param_name, FName scalar_param_name, FName vector_param_name, int material_element, FLinearColor color, UMaterialInterface* mi);

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	bool End(const FString& mesh_name);

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	bool Restore(const FString& mesh_name);

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	void Reset();

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	bool SetTextureParam(const FString& mesh_name, const int& material_element, UTexture2D* tex);

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	bool SetScalarParam(const FString& mesh_name, const int& material_element, const float& v);

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	bool SetVectorParam(const FString& mesh_name, const int& material_element, const FLinearColor& v);
};
