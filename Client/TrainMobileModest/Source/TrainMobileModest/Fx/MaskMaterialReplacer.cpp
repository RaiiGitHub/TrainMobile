// As Part of GuangZhou Training.

#include "MaskMaterialReplacer.h"
#include "TrainMobileModest.h"
#include "TextureFinder.h"


// Sets default values
AMaskMaterialReplacer::AMaskMaterialReplacer()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AMaskMaterialReplacer::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMaskMaterialReplacer::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

bool AMaskMaterialReplacer::AddMeshComponent(
	const FString & mesh_name, 
	const FString& mask_tex_path, 
	FName origin_tex_param_name, 
	FName mask_tex_param_name, 
	FName origin_scalar_param_name, 
	FName mask_scalar_param_name, 
	int material_element, 
	float origin_scarlar_val, 
	float mask_scarlar_val, 
	UMaterialInterface * mi)
{
	AStaticMeshActor* actor = FindObject<AStaticMeshActor>(ANY_PACKAGE, *mesh_name);
	if (nullptr == actor)
		return false;
	FMaskMesh mm;
	mm.mesh = actor->GetStaticMeshComponent();
	mm.materialIndex = material_element;
	FMaskMesh* p_mm = MaskMeshes.FindByKey(mm);
	if (nullptr == p_mm)						
	{
		mm.meshName = mesh_name;
		mm.miOrigin = mm.mesh->GetMaterial(material_element);
		mm.miTemplate = mm.mesh->CreateDynamicMaterialInstance(material_element, mi);
		mm.originTexParamName = origin_tex_param_name;
		mm.scalarOriginParamName = origin_scalar_param_name;
		mm.scalarMaskParamName= mask_scalar_param_name;
		mm.scalarMaskVal = mask_scarlar_val;
		mm.scalarOriginVal = origin_scarlar_val;
		mm.maskTexPath = mask_tex_path;
		mm.maskTexParamName = mask_tex_param_name;
		mm.masked = true;
		TArray<UTexture*> texs;
		mm.miOrigin->GetUsedTextures(texs, EMaterialQualityLevel::Medium, true, ERHIFeatureLevel::ES3_1, true);
		if (texs.Num() > 0)
			mm.miTemplate->SetTextureParameterValue(mm.originTexParamName, texs[0]);
		p_mm = &MaskMeshes[MaskMeshes.AddUnique(mm)];	
	}
	else
		p_mm->masked = true;
	//
	p_mm->miTemplate->SetScalarParameterValue(p_mm->originTexParamName,p_mm->scalarOriginVal);
	p_mm->miTemplate->SetScalarParameterValue(p_mm->scalarMaskParamName, p_mm->scalarMaskVal);
	ATextureFinder* tf = FindObject<ATextureFinder>(ANY_PACKAGE, TEXT("TextureFinder"));
	if (tf)
	{
		UTexture2D* tex = tf->LoadTexture(p_mm->maskTexPath);
		if( tex )
			p_mm->miTemplate->SetTextureParameterValue(p_mm->maskTexParamName, tex);
	}
	p_mm->mesh->SetMaterial(p_mm->materialIndex, p_mm->miTemplate);
	return true;
}

bool AMaskMaterialReplacer::RestoreMeshComponent(const FString & mesh_name, int material_element)
{
	AStaticMeshActor* actor = FindObject<AStaticMeshActor>(ANY_PACKAGE, *mesh_name);
	if (nullptr == actor)
		return false;
	FMaskMesh mm;
	mm.mesh = actor->GetStaticMeshComponent();
	mm.materialIndex = material_element;
	FMaskMesh* p_mm = MaskMeshes.FindByKey(mm);
	if (nullptr == p_mm || nullptr == p_mm->mesh)
		return false;
	p_mm->mesh->SetMaterial(p_mm->materialIndex, p_mm->miOrigin);
	return true;
}

bool AMaskMaterialReplacer::ReMaskMeshComponent(const FString & mesh_name, int material_element)
{
	AStaticMeshActor* actor = FindObject<AStaticMeshActor>(ANY_PACKAGE, *mesh_name);
	if (nullptr == actor)
		return false;
	FMaskMesh mm;
	mm.mesh = actor->GetStaticMeshComponent();
	mm.materialIndex = material_element;
	FMaskMesh* p_mm = MaskMeshes.FindByKey(mm);
	if (nullptr == p_mm || nullptr == p_mm->mesh)
		return false;
	p_mm->mesh->SetMaterial(p_mm->materialIndex, p_mm->miTemplate);
	return true;
}

bool AMaskMaterialReplacer::RemoveMeshComponent(const FString & mesh_name, int material_element)
{
	AStaticMeshActor* actor = FindObject<AStaticMeshActor>(ANY_PACKAGE, *mesh_name);
	if (nullptr == actor)
		return false;
	FMaskMesh mm;
	mm.mesh = actor->GetStaticMeshComponent();
	mm.materialIndex = material_element;
	FMaskMesh* p_mm = MaskMeshes.FindByKey(mm);
	if (p_mm && p_mm->mesh)
		p_mm->mesh->SetMaterial(p_mm->materialIndex, p_mm->miOrigin);
	MaskMeshes.Remove(mm);
	return true;
}

bool AMaskMaterialReplacer::MaskEntireMesh(
	const FString & mesh_name, 
	const FString& mask_tex_path, 
	FName origin_tex_param_name, 
	FName mask_tex_param_name, 
	FName origin_scalar_param_name, 
	FName mask_scalar_param_name, 
	float origin_scarlar_val, 
	float mask_scarlar_val, 
	UMaterialInterface * mi)
{
	AStaticMeshActor* actor = FindObject<AStaticMeshActor>(ANY_PACKAGE, *mesh_name);
	if (nullptr == actor)
		return false;
	UStaticMeshComponent* mesh = actor->GetStaticMeshComponent();
	int material_num = mesh->GetNumMaterials();
	for (int material_index = 0; material_index < material_num; ++material_index)
	{
		AddMeshComponent(mesh_name, mask_tex_path,
			origin_tex_param_name, mask_tex_param_name,
			origin_scalar_param_name, mask_scalar_param_name,
			material_index, origin_scarlar_val, mask_scarlar_val, mi);
	}
	return true;
}

bool AMaskMaterialReplacer::RestoreEntireMesh(const FString & mesh_name)
{
	AStaticMeshActor* actor = FindObject<AStaticMeshActor>(ANY_PACKAGE, *mesh_name);
	if (nullptr == actor)
		return false;
	UStaticMeshComponent* mesh = actor->GetStaticMeshComponent();
	int material_num = mesh->GetNumMaterials();
	for (int material_index = 0; material_index < material_num; ++material_index)
		RestoreMeshComponent(mesh_name, material_index);
	return true;
}

bool AMaskMaterialReplacer::ReMaskEntireMesh(const FString & mesh_name)
{
	AStaticMeshActor* actor = FindObject<AStaticMeshActor>(ANY_PACKAGE, *mesh_name);
	if (nullptr == actor)
		return false;
	UStaticMeshComponent* mesh = actor->GetStaticMeshComponent();
	int material_num = mesh->GetNumMaterials();
	for (int material_index = 0; material_index < material_num; ++material_index)
		ReMaskMeshComponent(mesh_name, material_index);
	return true;
}

bool AMaskMaterialReplacer::RemoveEntireMesh(const FString & mesh_name)
{
	AStaticMeshActor* actor = FindObject<AStaticMeshActor>(ANY_PACKAGE, *mesh_name);
	if (nullptr == actor)
		return false;
	UStaticMeshComponent* mesh = actor->GetStaticMeshComponent();
	int material_num = mesh->GetNumMaterials();
	for (int material_index = 0; material_index < material_num; ++material_index)
		RemoveMeshComponent(mesh_name, material_index);
	return true;
}

