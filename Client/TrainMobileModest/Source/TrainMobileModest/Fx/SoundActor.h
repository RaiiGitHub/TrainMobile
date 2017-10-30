// As Part of GuangZhou Training.

#pragma once

#include "GameFramework/Actor.h"
#include "SoundActor.generated.h"

UCLASS()
class TRAINMOBILEMODEST_API ASoundActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASoundActor();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

private:
	struct SoundAudio {
		class USoundBase* sound;
		class UAudioComponent* audio;
		bool operator < (const SoundAudio& sa) const
		{
			return sound < sa.sound;
		}
		bool operator == (const SoundAudio& sa) const
		{
			return sound == sa.sound;
		}
		SoundAudio(USoundBase* s, UAudioComponent* a) :sound(s), audio(a) {}
	};
	TArray<SoundAudio> Sounds;
public:
	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	USoundBase* GetSound(const FString& sound_path);

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	bool PlaySound2D(const FString& sound_path,float VolumeMultiplier = 1.f, float PitchMultiplier = 1.f, float StartTime = 0.f );

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	bool StopSound(const FString& sound_path);
};
