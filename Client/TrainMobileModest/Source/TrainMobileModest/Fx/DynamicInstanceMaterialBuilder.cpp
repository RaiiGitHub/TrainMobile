// As Part of GuangZhou Training.

#include "DynamicInstanceMaterialBuilder.h"
#include "TrainMobileModest.h"


// Sets default values
ADynamicInstanceMaterialBuilder::ADynamicInstanceMaterialBuilder()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ADynamicInstanceMaterialBuilder::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ADynamicInstanceMaterialBuilder::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

bool ADynamicInstanceMaterialBuilder::Begin(const FString & mesh_name, UMeshComponent* mesh, FName tex_param_name, FName scalar_param_name, FName vector_param_name, int material_element, FLinearColor color, UMaterialInterface * mi)
{
	return Dm.Begin(mesh_name, mesh,tex_param_name, scalar_param_name, vector_param_name, material_element, color, mi);
}

bool ADynamicInstanceMaterialBuilder::End(const FString & mesh_name)
{
	return Dm.End(mesh_name);
}

bool ADynamicInstanceMaterialBuilder::Restore(const FString & mesh_name)
{
	//empty for clear all mesh.
	return Dm.Restore(mesh_name);
}

void ADynamicInstanceMaterialBuilder::Reset()
{
	Dm.Reset();
}

bool ADynamicInstanceMaterialBuilder::SetTextureParam(const FString & mesh_name, const int& material_element, UTexture2D* tex)
{
	return Dm.SetTextureParam(mesh_name, material_element, tex);
}

bool ADynamicInstanceMaterialBuilder::SetScalarParam(const FString & mesh_name, const int& material_element,const float& v)
{
	return Dm.SetScalarParam(mesh_name, material_element, v);
}

bool ADynamicInstanceMaterialBuilder::SetVectorParam(const FString & mesh_name, const int& material_element, const FLinearColor& v)
{
	return Dm.SetVectorParam(mesh_name, material_element, v);
}

