// As Part of GuangZhou Training.

#include "TrainMobileModest.h"
#include "TextureMesh.h"


// Sets default values
ATextureMesh::ATextureMesh()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ATextureMesh::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATextureMesh::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

bool ATextureMesh::ReShapeHorizonLineMesh(FVector start, FVector end, AStaticMeshActor * mesh, float offsetHeight, float uRate, const FString& collectionPath, FName scalarParamName)
{
	if (nullptr == mesh)
		return false;
	UMaterialParameterCollection* pc = LoadObject<UMaterialParameterCollection>(NULL,
		*collectionPath,
		NULL,
		LOAD_None,
		NULL);
	UMaterialParameterCollectionInstance* pci = GetWorld()->GetParameterCollectionInstance(pc);
	if (nullptr == pci)
		return false;
	FVector dirAndLen = end - start;
	FVector dir;
	float len;
	dirAndLen.ToDirectionAndLength(dir,len);
	float ha = dir.HeadingAngle();
	float uRepeate = len / uRate;
	ha = FMath::RadiansToDegrees(ha);

	pci->SetScalarParameterValue(scalarParamName, uRepeate);
	FVector scale = mesh->GetActorScale3D();
	scale.X = len;
	mesh->SetActorScale3D(scale);
	FRotator rotate = mesh->GetActorRotation();
	rotate.Yaw = ha;
	FVector location = (end + start)*.5f;
	location.Z += offsetHeight;
	mesh->SetActorLocation(location);
	mesh->SetActorRotation(rotate);
	//OutputDebugString(*FString::Printf(TEXT("heading:%f\n"), ha));
	return true;
}

