// As Part of GuangZhou Training.

#include "SoundActor.h"
#include "TrainMobileModest.h"



// Sets default values
ASoundActor::ASoundActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void ASoundActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASoundActor::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

USoundBase * ASoundActor::GetSound(const FString & sound_path)
{
	USoundBase* sound = LoadObject<USoundBase>(nullptr, *sound_path);
	if (nullptr != sound)
	{
		Sounds.AddUnique(SoundAudio(sound,nullptr));
		return sound;
	}
	return nullptr;
}

bool ASoundActor::PlaySound2D(const FString & sound_path, float VolumeMultiplier, float PitchMultiplier, float StartTime)
{
	USoundBase* sound = GetSound(sound_path);
	if (nullptr == sound)
		return false;
	SoundAudio sa(sound, nullptr);
	SoundAudio* fsa = Sounds.FindByKey(sa);
	if (nullptr == fsa)
		return false;
	if (fsa->audio )
	{
		fsa->audio->Play(StartTime);
		return true;
	}
	UAudioComponent* audio = UGameplayStatics::SpawnSound2D(this,sound,VolumeMultiplier,PitchMultiplier,StartTime);
	if (audio)
		fsa->audio = audio;
	return true;
}

bool ASoundActor::StopSound(const FString & sound_path)
{
	USoundBase* sound = GetSound(sound_path);
	if (nullptr == sound)
		return false;
	SoundAudio sa(sound, nullptr);
	SoundAudio* fsa = Sounds.FindByKey(sa);
	if (nullptr == fsa || nullptr == fsa->audio )
		return false;
	fsa->audio->Stop();
	return true;
}

