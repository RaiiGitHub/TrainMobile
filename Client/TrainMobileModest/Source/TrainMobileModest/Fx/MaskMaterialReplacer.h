// As Part of GuangZhou Training.

#pragma once

#include "GameFramework/Actor.h"
#include "MaskMaterialReplacer.generated.h"

UCLASS()
class TRAINMOBILEMODEST_API AMaskMaterialReplacer : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMaskMaterialReplacer();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;
public:
	struct FMaskMesh {
		UMeshComponent* mesh;
		UMaterialInterface* miOrigin;
		UMaterialInstanceDynamic* miTemplate;
		int materialIndex;
		float scalarMaskVal;
		float scalarOriginVal;
		FName scalarOriginParamName;
		FName scalarMaskParamName;
		FName originTexParamName;
		FName maskTexParamName;
		FString meshName;
		FString maskTexPath;
		bool masked;
		FMaskMesh() :mesh(nullptr), miOrigin(nullptr), miTemplate(nullptr), materialIndex(0), masked(false), scalarMaskVal(0), scalarOriginVal(0){}
		bool operator < (const FMaskMesh& mm) const
		{
			return mesh < mm.mesh && materialIndex < mm.materialIndex;
		}
		bool operator == (const FMaskMesh& mm) const
		{
			return mesh == mm.mesh
				&& materialIndex == mm.materialIndex;
		}
	};
private:
	TArray<FMaskMesh> MaskMeshes;
public:
	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	bool AddMeshComponent(const FString& mesh_name, const FString& mask_tex_path, FName origin_tex_param_name, FName mask_tex_param_name, FName origin_scalar_param_name, FName mask_scalar_param_name, int material_element, float origin_scarlar_val, float mask_scarlar_val,UMaterialInterface* mi);
	
	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	bool RestoreMeshComponent(const FString& mesh_name, int material_element);

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	bool ReMaskMeshComponent(const FString& mesh_name, int material_element);

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	bool RemoveMeshComponent(const FString& mesh_name, int material_element);

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	bool MaskEntireMesh(const FString& mesh_name, const FString& mask_tex_path, FName mask_tex_param_name, FName origin_tex_param_name, FName origin_scalar_param_name, FName mask_scalar_param_name, float origin_scarlar_val, float mask_scarlar_val, UMaterialInterface* mi);

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	bool RestoreEntireMesh(const FString& mesh_name);

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	bool ReMaskEntireMesh(const FString& mesh_name);

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	bool RemoveEntireMesh(const FString& mesh_name);
};
