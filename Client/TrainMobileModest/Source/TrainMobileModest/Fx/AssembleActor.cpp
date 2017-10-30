// As Part of GuangZhou Training.

#include "TrainMobileModest.h"
#include "AssembleActor.h"
#include "TrainGameInstance.h"


// Sets default values
AAssembleActor::AAssembleActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	RootComponent = CreateDefaultSubobject<USceneComponent>(FName(*FString::Printf(TEXT("AssembleActorRoot"), this)));
}

// Called when the game starts or when spawned
void AAssembleActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AAssembleActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

static const FString AssembleChildActorName(TEXT("AAssembleChildActor_Spawn_"));
static const FString AssembleMeshName(TEXT("AAssembleMesh_Spawn_"));

bool AAssembleActor::SetupAssembleMeshWithSockets(UStaticMeshComponent* mesh_component, const TArray<FGameKeyValue>& sokcet_mesh_src)
{
	if (nullptr == mesh_component)
		return false;

	static int spawn_index = 0;
	UStaticMesh* sm = mesh_component->GetStaticMesh();
	if (nullptr == sm)
		return false;
	AActor* owner = mesh_component->GetOwner();
	if (nullptr == owner || nullptr == owner->GetRootComponent())
		return false;

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	Params.bAllowDuringConstructionScript = true;
	Params.OverrideLevel = owner->GetLevel();
	TArray<USceneComponent*> childComponents;
	owner->GetRootComponent()->GetChildrenComponents(true, childComponents);
	for (auto& cc : childComponents)
	{
		if (cc->GetName().Contains(AssembleMeshName))
		{
			cc->GetOwner()->Destroy();
			cc->RemoveFromRoot();
		}
	}
	int socket_index = 0;
	for (UStaticMeshSocket* Socket : sm->Sockets)
	{
		FString mesh_src;
		if (GameUtilityTool::GetValueInGameKeyValue(sokcet_mesh_src,Socket->SocketName.ToString(),mesh_src))
		{
			UStaticMesh* socket_sm = LoadObject<UStaticMesh>(GetLevel(),
				*mesh_src,
				NULL,
				LOAD_None,
				NULL);
			if (nullptr != socket_sm )
			{
				Params.Name = FName(*FString::Printf(TEXT("%s%d"), *AssembleChildActorName, spawn_index++));
				AActor* _sa = GetWorld()->SpawnActor(TSubclassOf<AActor>(AActor::StaticClass()), 0, 0, Params);
				UStaticMeshComponent* _smc = NewObject<UStaticMeshComponent>(_sa, FName(*FString::Printf(TEXT("%s%d"), *AssembleMeshName, spawn_index++)));
				_smc->SetStaticMesh(socket_sm);
				_smc->SetCollisionProfileName(UCollisionProfile::DefaultProjectile_ProfileName);
				_smc->RegisterComponent();
				_sa->SetRootComponent(_smc);
				FMatrix SocketTM;
				if (Socket->GetSocketMatrix(SocketTM, _smc))
				{
					_sa->Modify();
					_sa->SetActorLocation(SocketTM.GetOrigin(), false);
					_sa->SetActorRotation(SocketTM.Rotator());
				}
				_sa->AttachToActor(owner, FAttachmentTransformRules::KeepRelativeTransform);
			}
		}
	}
	return true;
}

TArray<AActor*> AAssembleActor::GetAssembleChildActors(UStaticMeshComponent* mesh_component)
{
	TArray<AActor*> actors;
	if (nullptr == mesh_component)
		return actors;
	AActor* owner = mesh_component->GetOwner();
	if (nullptr == owner || nullptr == owner->GetRootComponent())
		return actors;
	TArray<USceneComponent*> childComponents;
	owner->GetRootComponent()->GetChildrenComponents(true, childComponents);
	for (auto& cc : childComponents)
	{
		if (cc->GetName().Contains(AssembleMeshName))
			actors.Add(cc->GetOwner());
	}
	return actors;
}

