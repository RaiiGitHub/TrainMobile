// As Part of GuangZhou Training.

#pragma once

#include "GameFramework/Actor.h"
#include "DynamicMaterial.h"
#include "DynamicMaterialOnMesh.generated.h"




UCLASS()
class TRAINMOBILEMODEST_API ADynamicMaterialOnMesh : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADynamicMaterialOnMesh();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;
private:
	DynamicMaterial Dm;
public:
	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	void SetupBreathMethod(float interval, float peak,EBreathMethod bmt = EBreathMethod::BREATH_METHOD_EXP);

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	void Reset();

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	void SetFixEnergy(float atWhatTime);

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	void SetDynamicEnergy();

	//effects, emissiving.
	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	bool Begin(const FString& mesh_name, FName tex_param_name, FName scalar_param_name, FName vector_param_name, int material_element, FLinearColor color, UMaterialInterface* mi);

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	bool BeginWithAllElement(const FString& mesh_name, FName tex_param_name, FName scalar_param_name, FName vector_param_name, FLinearColor color, UMaterialInterface* mi);

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	bool End(const FString& mesh_name);

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	bool Restore(const FString& mesh_name);

};