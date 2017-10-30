// As Part of GuangZhou Training.

#include "TrainMobileModest.h"
#include "SpriteActor.h"
#include "PawnHandler.h"
#include "DisplayContent.h"

// Sets default values
ASpriteActor::ASpriteActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PawnHandler = nullptr;
	//create components
	SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));
	SphereCollision->SetupAttachment(RootComponent);
	SphereCollision->SetSphereRadius(60);
	SphereCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	ParticleSystem = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ParticleSystem"));
	ParticleSystem->SetupAttachment(SphereCollision);
	ParticleSystem->bAutoActivate = false;
	ParticleSystem->SetRelativeLocation(FVector(0.f, 0.f, 0.f));
	static ConstructorHelpers::FObjectFinder<UParticleSystem> ParticleAsset(TEXT("ParticleSystem'/Game/Particles/Particles/P_Selected_Title.P_Selected_Title'"));
	if (ParticleAsset.Succeeded())
		ParticleSystem->SetTemplate(ParticleAsset.Object);
	
	SpriteBillboard = CreateDefaultSubobject<UBillboardComponent>(TEXT("SpriteBillboard"));
	SpriteBillboard->SetupAttachment(SphereCollision);
	SpriteBillboard->SetHiddenInGame(true);
	EnsureParticlVisible = 0.f;

}

// Called when the game starts or when spawned
void ASpriteActor::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ASpriteActor::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
}

bool ASpriteActor::HitOnSpriteTest()
{
	if( nullptr == PawnHandler )
		PawnHandler = FindObject<APawnHandler>(ANY_PACKAGE, TEXT("PawnHandler"));
	if (nullptr == PawnHandler)
		return false;
	FHitResult hr;
	if (PawnHandler->GetHitResult(hr))
	{
		if (this == hr.GetActor())
			return true;
	}
	return false;
}

FHitResult & ASpriteActor::GetHitResult()
{
	// TODO: insert return statement here
	// warning,this function should only be used for testing.
	static APawnHandler* ph = FindObject<APawnHandler>(ANY_PACKAGE, TEXT("PawnHandler"));
	if (nullptr == ph)
		return HitResult;
	ph->GetHitResult(HitResult);
	return HitResult;
}

void ASpriteActor::SpriteGetAwayPlease(const FString& tellMeSth)
{
	if (SphereCollision)
		SphereCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	if (ParticleSystem)
	{
		ParticleSystem->SetRelativeLocation(FVector(0.f, 0.f, 0.f));
		ParticleSystem->SetActive(false);
	}
	ADisplayContent* dc = FindObject<ADisplayContent>(ANY_PACKAGE, TEXT("ContentHistory"));
	if (dc && !tellMeSth.IsEmpty() )
		dc->AddContent(tellMeSth);
}

void ASpriteActor::SpriteComeOutPlease(const FString & tellMeSth)
{
	if (SphereCollision)
		SphereCollision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	if (ParticleSystem)
		ParticleSystem->SetActive(true);
	ADisplayContent* dc = FindObject<ADisplayContent>(ANY_PACKAGE, TEXT("ContentHistory"));
	if (dc && !tellMeSth.IsEmpty())
		dc->AddContent(tellMeSth);

}

bool ASpriteActor::SetupSpriteRelativeLocation(const AActor * container)
{
	if (nullptr == container || nullptr == ParticleSystem)
		return false;
	FVector origin;
	FVector boxExtend;
	container->GetActorBounds(true, origin, boxExtend);
	ParticleSystem->SetRelativeLocation(FVector(0.f, 0.f, boxExtend.Z));
	return true;
}

void ASpriteActor::Reset()
{
	HitResult.Reset();
}

