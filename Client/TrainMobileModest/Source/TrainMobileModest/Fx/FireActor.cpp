// As Part of GuangZhou Training.

#include "FireActor.h"
#include "TrainMobileModest.h"
#include "CallbackActor.h"
#include "Particles/Spawn/ParticleModuleSpawn.h"

AFireActor::AFireActor()
{
	PrimaryActorTick.bCanEverTick = true;

	//particle system
	ParticleSystem = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ParticleSystem"));
	ParticleSystem->bAutoActivate = false;
	ParticleSystem->SetRelativeLocation(FVector(-20.0f, 0.0f, 20.0f));
	RootComponent = ParticleSystem;

	//audio component
	ParticleAudio = CreateDefaultSubobject<UAudioComponent>(TEXT("ParticleAudio"));
	ParticleAudio->SetupAttachment(ParticleSystem);
	ParticleAudio->bAutoActivate = false;

	FireChangeType = FIRE_CHANGE_PARAM_UNKNOWN;
	ReachPercent = 0.2f;
	ChangeRate = 0.05f;//20s to complete 100%.
	CurPercent = 0.f;
	bIsPause = false;
	BenchPercent = 1.f;
}

void AFireActor::BeginPlay()
{
	Super::BeginPlay();
}

void AFireActor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (bIsPause)
		return;
	bool needToApply = false;
	int reachTargetPercent = 0;
	if (FIRE_CHANGE_PARAM_WEAK == FireChangeType)
	{
		float deltaPercent = DeltaSeconds * ChangeRate;
		if (ReachPercent >= CurPercent - deltaPercent)
		{
			FireChangeType = FIRE_CHANGE_PARAM_UNKNOWN;
			CurPercent = ReachPercent;
			reachTargetPercent = 1;
			//dc->AddContent(TEXT("已将火势减弱到指定的值，要完全灭掉，请执行接下来的任务。"));
		}
		else
			CurPercent -= deltaPercent;
		needToApply = true;
	}
	else if (FIRE_CHANGE_PARAM_ENHANCE == FireChangeType)
	{
		float deltaPercent = DeltaSeconds * ChangeRate;
		if (ReachPercent <= CurPercent + deltaPercent)
		{
			FireChangeType = FIRE_CHANGE_PARAM_UNKNOWN;
			CurPercent = ReachPercent;
			reachTargetPercent = 2;
			//dc->AddContent(TEXT("火势已增加到指定的值。"));
		}
		else
			CurPercent += deltaPercent;
		needToApply = true;
	}
	if(needToApply)
	{
		ParticleSystem->SetVectorParameter(TEXT("FireSizeParameter"), FVector(BenchPercent*CurPercent, BenchPercent*CurPercent, BenchPercent*CurPercent));
		ParticleSystem->SetVectorParameter(TEXT("SmokeSizeParameter"), FVector(BenchPercent*CurPercent*.5f, BenchPercent*CurPercent*.5f, BenchPercent*CurPercent*.5f));
		ParticleAudio->SetVolumeMultiplier(BenchPercent*CurPercent*.75f);

		//notify fire_percent
		ACallbackActor* callbackActor = FindObject<ACallbackActor>(ANY_PACKAGE, TEXT("CallbackActor"));
		if (callbackActor)
		{
			callbackActor->SetCallbackType(Callback_NotifyFirePercent);
			callbackActor->NotifyFirePercent(CurPercent, reachTargetPercent);
		}
	}
}

void AFireActor::SetActiveFire(bool bActive, bool bReset)
{
	if (ParticleSystem && ParticleSystem->Template)
	{
		ParticleSystem->SetActive(bActive, bReset);
		if (ParticleAudio)
			ParticleAudio->SetActive(bActive, bReset);
		FireChangeType = FIRE_CHANGE_PARAM_UNKNOWN;
		FVector fs(0, 0, 0);
		ParticleSystem->GetVectorParameter(TEXT("FireSizeParameter"), fs);
		BenchPercent = .3f*(fs.X + fs.Y + fs.Z);
		CurPercent = 1.f;
	}
}

void AFireActor::SetReachPercent(float fReachPercent)
{
	ReachPercent = fReachPercent;
}

float AFireActor::GetFirePercent()
{
	return CurPercent;
}

bool AFireActor::WeakFire()
{
	if (ParticleSystem && ParticleSystem->Template)
	{
		FireChangeType = FIRE_CHANGE_PARAM_WEAK;
		return true;
	}
	return false;
}

bool AFireActor::EnhanceFire()
{
	if (ParticleSystem && ParticleSystem->Template)
	{
		FireChangeType = FIRE_CHANGE_PARAM_ENHANCE;
		return true;
	}
	return false;
}

void AFireActor::Pause()
{
	bIsPause = true;
}

void AFireActor::Resume()
{
	bIsPause = false;
}
