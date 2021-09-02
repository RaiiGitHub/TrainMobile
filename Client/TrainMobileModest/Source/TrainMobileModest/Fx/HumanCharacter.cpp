// As Part of GuangZhou Training.

#include "HumanCharacter.h"
#include "TrainMobileModest.h"
#include "RoleFollowLabelComponent.h"
#include "SpriteActor.h"
#include "PawnHandler.h"
#include "GameFramework/InputSettings.h"  
#include "Kismet/GameplayStatics.h"
#include "TrainGameInstance.h"
#include "SocketProducer.h"

// Sets default values
AHumanCharacter::AHumanCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
	GetMesh()->SetRelativeLocationAndRotation(FVector(0, 0, -97), FRotator(0, 90, 0));
	// set our turn rates for input
	bJumpButtonDown = false;
	bTakeOnObject = false;
	bCanEmitParticle = false;
	bPrepareToOpenParticleEffect = false;
	bParticleEffectOpen = false;
	HoldOnActor = nullptr;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;


	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 9.f;
	GetCharacterMovement()->AirControl = 0.2f;
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;


	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 280.f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller
	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with leve

	DefaultArmLength = CameraBoom->TargetArmLength;

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm
	FollowCamera->SetRelativeRotation(FRotator(-20,0,0));
	FollowCamera->SetRelativeLocation(FVector(0, 0, 150));

	//create mesh slot scene component
	MeshSlot = CreateDefaultSubobject<USceneComponent>(TEXT("MeshSlot"));
	MeshSlot->SetRelativeLocation(FVector(0, 0, 0));

	//create static mesh component
	MeshStaticObject = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshStaticObject"));
	MeshStaticObject->SetupAttachment(MeshSlot);
	MeshStaticObject->bCastDynamicShadow = true;
	MeshStaticObject->CastShadow = true;
	MeshStaticObject->SetRelativeLocationAndRotation(FVector(39.264015, -23.918957, -15.51705), FRotator(-72.325127, 124.104874, 24.794298));

	//particle system
	ParticleSystem = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ParticleSystem"));
	ParticleSystem->SetupAttachment(MeshSlot);
	ParticleSystem->bAutoActivate = false;
	ParticleSystem->SetRelativeLocation(FVector(62.306995, 7.413736, 7.477849));

	//audio component
	ParticleAudio = CreateDefaultSubobject<UAudioComponent>(TEXT("ParticleAudio"));
	ParticleAudio->SetupAttachment(ParticleSystem);
	ParticleAudio->bAutoActivate = false;

	//label
	RoleLabel = CreateDefaultSubobject<URoleFollowLabelComponent>(TEXT("RoleLabel"));
	RoleLabel->SetRoleLabelText(FText::FromString(TEXT("")));
	RoleLabel->SetupAttachment(GetCapsuleComponent());
	RoleLabel->SetRelativeLocationAndRotation(FVector(0, 0, 100.f), FRotator(0, -70, 0));

	//sprite
	SpriteCollision1 = CreateDefaultSubobject<USphereComponent>(TEXT("SpriteCollision1"));
	SpriteCollision1->SetupAttachment(GetCapsuleComponent());
	SpriteCollision1->SetSphereRadius(25);
	SpriteCollision1->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Sprite1 = CreateDefaultSubobject<UBillboardComponent>(TEXT("Sprite1"));
	Sprite1->SetupAttachment(SpriteCollision1);
	Sprite1->SetHiddenInGame(true);

	SpriteCollision2 = CreateDefaultSubobject<USphereComponent>(TEXT("SpriteCollision2"));
	SpriteCollision2->SetupAttachment(GetCapsuleComponent());
	SpriteCollision2->SetSphereRadius(25);
	SpriteCollision2->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Sprite2 = CreateDefaultSubobject<UBillboardComponent>(TEXT("Sprite2"));
	Sprite2->SetupAttachment(SpriteCollision2);
	Sprite2->SetHiddenInGame(true);

	SpriteCollision3 = CreateDefaultSubobject<USphereComponent>(TEXT("SpriteCollision3"));
	SpriteCollision3->SetupAttachment(GetCapsuleComponent());
	SpriteCollision3->SetSphereRadius(25);
	SpriteCollision3->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Sprite3 = CreateDefaultSubobject<UBillboardComponent>(TEXT("Sprite3"));
	Sprite3->SetupAttachment(SpriteCollision3);
	Sprite3->SetHiddenInGame(true);

	VelocityIgnoreCollision = 0.f;
	VelocityManualValueSend = 0.f;
}

// Called when the game starts or when spawned
void AHumanCharacter::BeginPlay()
{
	Super::BeginPlay();
	FVector right = GetActorRightVector();
	FVector up = GetActorUpVector();
	FVector origin;
	FVector boxExtend;
	GetActorBounds(true, origin, boxExtend);
	float   Sprite1Radius = SpriteCollision1->GetScaledSphereRadius();
	float   Sprite2Radius = SpriteCollision2->GetScaledSphereRadius();
	FVector Sprite1Pos = origin + up * 12.f + right*(boxExtend.X*.5f);
	FVector Sprite2Pos = Sprite1Pos + right * Sprite1Radius;
	FVector Sprite3Pos = Sprite2Pos + right * Sprite2Radius;

	if (SpriteCollision1)
		SpriteCollision1->SetWorldLocation(Sprite1Pos);
	if (SpriteCollision2)
		SpriteCollision2->SetWorldLocation(Sprite2Pos);
	if (SpriteCollision3)
		SpriteCollision3->SetWorldLocation(Sprite3Pos);

	DefaultCameraRoator = GetActorRotation();
}

// Called every frame
void AHumanCharacter::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
	if (bCanEmitParticle)
	{
		if (bParticleEffectOpen^bPrepareToOpenParticleEffect)
		{
			SetActiveParticle(bPrepareToOpenParticleEffect);
			bParticleEffectOpen = bPrepareToOpenParticleEffect;
		}
	}
	//jump forward
	if (bJumpButtonDown)
	{
		FVector l = GetActorLocation() + 10.f*GetActorForwardVector() + 5.f*GetActorUpVector();// jumping up and forward.
		SetActorLocation(l);
	}
	if (RoleLabel)
		RoleLabel->UpdateRoleLabel();
}

bool AHumanCharacter::SetActiveParticle(bool active)
{
	//static mesh should be visible
	active &= (MeshStaticObject && MeshStaticObject->IsVisible());
	if (ParticleSystem && ParticleSystem->Template)
	{
		active?ParticleSystem->AttachToComponent(MeshSlot,FAttachmentTransformRules::KeepRelativeTransform): 
			ParticleSystem->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
		ParticleSystem->SetActive(active, true);
		if (ParticleAudio)
			ParticleAudio->SetActive(active, true);
		FixHandupPose(!active);
		return true;
	}
	return false;
}

void AHumanCharacter::JumpPress()
{
	bJumpButtonDown = CanJump();
	if (bJumpButtonDown)
	{
		ACharacter::Jump();
		UPawnMovementComponent* pmc = GetMovementComponent();
		if (pmc)
		{
			UCharacterMovementComponent *cmc = static_cast<UCharacterMovementComponent*>(pmc);
			cmc->JumpZVelocity = 9.f;
		}
	}
}

void AHumanCharacter::JumpRelease()
{
	if (bJumpButtonDown)
	{
		bJumpButtonDown = false;
		ACharacter::StopJumping();
	}
}

APawnHandler* AHumanCharacter::GetPawnHandler()
{
	APawnHandler* ph = FindObject<APawnHandler>(ANY_PACKAGE, TEXT("PawnHandler"));
	if (nullptr == ph)
		return nullptr;
	if (ph->GetCurrentHandlePawn() != this)
		return nullptr;
	return ph;
}

bool AHumanCharacter::QueryLogicKey(const FString & key)
{
	bool* val = LogicKeys.Find(key);
	if (nullptr == val)
		return false;
	return *val;
}

bool AHumanCharacter::ParticleIsActive()
{
	return bParticleEffectOpen;
}

void AHumanCharacter::ParticlePress()
{	
	bPrepareToOpenParticleEffect = true;
}

void AHumanCharacter::ParticleRelease()
{
	bPrepareToOpenParticleEffect = false;
	bParticleEffectOpen = false;
	SetActiveParticle(false);
}

void AHumanCharacter::FixHandupPose(bool as_normal)
{
	if (nullptr == MeshStaticObject)
		return;
	if (as_normal)
	{
		MeshStaticObject->SetRelativeLocationAndRotation(FVector(39.264015, -23.918957, -15.51705), FRotator(-72.325127, 124.104874, 24.794298));
		//change to another object.
		AStaticMeshActor* actor = FindObject<AStaticMeshActor>(ANY_PACKAGE, TEXT("miehuoqi"));
		if (nullptr != actor)
			MeshStaticObject->SetStaticMesh(actor->GetStaticMeshComponent()->GetStaticMesh());
	}
	else
	{
		MeshStaticObject->SetRelativeLocationAndRotation(FVector(-22.239243, -8.095934, -48.913654), FRotator(19.092146, 146.170944, 21.728527));
		//change to another object.
		AStaticMeshActor* actor = FindObject<AStaticMeshActor>(ANY_PACKAGE, TEXT("miehuoqi_work"));
		if ( nullptr != actor)
			MeshStaticObject->SetStaticMesh(actor->GetStaticMeshComponent()->GetStaticMesh());
	}
}

TMap<FString, bool>& AHumanCharacter::GetLogicKeys()
{
	// TODO: insert return statement here
	return LogicKeys;
}

AStaticMeshActor * AHumanCharacter::GetHoldOnMesh()
{
	return HoldOnActor;
}

void AHumanCharacter::SetHoldOnMesh(AStaticMeshActor * mesh)
{
	HoldOnActor = mesh;
	bTakeOnObject = nullptr != mesh;
	if (nullptr == mesh)
		MeshSlot->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	else
	{
		MeshSlot->AttachToComponent(GetMesh(),
			FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true),
			TEXT("BindSocket0"));
	}
}

//network
void AHumanCharacter::FSynchronizeSender::SyncToServer(const FString& pawn_name)
{
	AHumanCharacter* hc = FindObject<AHumanCharacter>(ANY_PACKAGE, *pawn_name);
	if (nullptr == hc)
		return;
	UTrainGameInstance* ins = static_cast<UTrainGameInstance*>(hc->GetGameInstance());
	if (nullptr == ins)
		return;
	
	FString ani_set;
	ani_set.Append(FString::Printf(TEXT("jump|%d,"),  hc->bJumpButtonDown ? 1 : 0));
	ani_set.Append(FString::Printf(TEXT("particleprepare|%d,"), hc->bPrepareToOpenParticleEffect ? 1 : 0));
	for (auto& k : hc->GetLogicKeys())
		ani_set.Append(FString::Printf(TEXT("%s|%d,"), *k.Key, k.Value ? 1 : 0));
	ani_set.RemoveFromEnd(",");
	TArray<FString> data;
	data.Add(ins->BuildMovementString(
		hc->GetActorLocation(),
		hc->GetActorRotation(),
		hc->GetActorScale(),
		hc->GetMovementComponent()->Velocity,
		ani_set));
	hc->VelocityManualValueSend = hc->GetMovementComponent()->Velocity.Size();
	FString s = ins->BuildMovementSendString(pawn_name,data);
	if (!s.IsEmpty())
		ins->SendDataToSocketServer(FSocketProducer::DefaultSocketServerName, s, TEXT("forwardmovementbyuserid"), true);
}

void AHumanCharacter::FSynchronizeSender::SyncFromServer(const FString& pawn_name, const FString& ani_name, const FTransform &tf, const FVector& velocity)
{
	AHumanCharacter* hc = FindObject<AHumanCharacter>(ANY_PACKAGE, *pawn_name);
	if (nullptr == hc)
		return;
	UTrainGameInstance* ins = static_cast<UTrainGameInstance*>(hc->GetGameInstance());
	if (nullptr == ins)
		return;
	TMap<FString, bool>& anis = hc->GetLogicKeys();
	anis.Empty();

	FString cur = ani_name;
	bool end = false;
	do
	{
		FString l, r;
		if (!cur.Split(",", &l, &r))
		{
			end = true;
			l = cur;
		}
		FString _l, _r;
		if (l.Split("|", &_l, &_r))
		{
			bool v = "1" == _r;
			if ("jump" == _l)
				hc->bJumpButtonDown = v;
			else if ("particleprepare" == _l)
			{
				if (!v && hc->bPrepareToOpenParticleEffect)
					hc->ParticleRelease();
				else if (v && !hc->bPrepareToOpenParticleEffect)
					hc->ParticlePress();
			}
			else
				anis.FindOrAdd(_l) = v;
		}
		cur = r;
	} while (!end);
	hc->SetActorTransform(tf);
	//affect the collision-ignore velocity
	hc->VelocityIgnoreCollision = velocity.Size();
}