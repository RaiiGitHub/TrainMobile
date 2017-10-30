// As Part of GuangZhou Training.

#include "TrainMobileModest.h"
#include "DynamicMaterialOnMesh.h"


// Sets default values
ADynamicMaterialOnMesh::ADynamicMaterialOnMesh()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ADynamicMaterialOnMesh::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ADynamicMaterialOnMesh::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
	Dm.Tick(DeltaTime);
	
}

void ADynamicMaterialOnMesh::SetupBreathMethod(float interval, float peak, EBreathMethod bmt)
{
	Dm.SetupBreathMethod(interval, peak,bmt);
}

void ADynamicMaterialOnMesh::Reset()
{
	Dm.Reset();
}

void ADynamicMaterialOnMesh::SetFixEnergy(float atWhatTime)
{
	Dm.SetFixEnergy(atWhatTime);
}

void ADynamicMaterialOnMesh::SetDynamicEnergy()
{
	Dm.SetDynamicEnergy();
}

bool ADynamicMaterialOnMesh::Begin(const FString& mesh_name, FName tex_param_name,FName scalar_param_name, FName vector_param_name, int material_element, FLinearColor color, UMaterialInterface* mi)
{
	return Dm.Begin(mesh_name, tex_param_name,scalar_param_name, vector_param_name, material_element, color,mi);
}

bool ADynamicMaterialOnMesh::BeginWithAllElement(const FString & mesh_name, FName tex_param_name, FName scalar_param_name, FName vector_param_name, FLinearColor color, UMaterialInterface * mi)
{
	return Dm.BeginWithAllElement(mesh_name, tex_param_name,scalar_param_name, vector_param_name, color, mi);
}

bool ADynamicMaterialOnMesh::End(const FString & mesh_name)
{
	return Dm.End(mesh_name);
}

bool ADynamicMaterialOnMesh::Restore(const FString & mesh_name)
{
	return Dm.Restore(mesh_name);
}

