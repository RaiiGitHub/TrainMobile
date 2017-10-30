// As Part of GuangZhou Training.

#pragma once

#include "GameFramework/Actor.h"
#include "FireActor.generated.h"

/**
 * 
 */
UCLASS()
class TRAINMOBILEMODEST_API AFireActor : public AActor
{
	GENERATED_BODY()
public:
	AFireActor();
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;
public:
	/**mesh object attached to the mesh slot */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Particle)
	class UParticleSystemComponent *ParticleSystem;

	/*Audio Component*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Particle)
	class UAudioComponent *ParticleAudio;

private:
	enum {
		FIRE_CHANGE_PARAM_UNKNOWN = 0,
		FIRE_CHANGE_PARAM_WEAK = 1,
		FIRE_CHANGE_PARAM_ENHANCE = 2
	};
	int FireChangeType;
	float ReachPercent;
	float CurPercent;
	float ChangeRate;//percent/s.
	float BenchPercent;//at very begin.
	bool  bIsPause;
public:
	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	void SetActiveFire(bool bActive,bool bReset = false);

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	void SetReachPercent(float fReachPercent);

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	float GetFirePercent();

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	bool WeakFire();

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	bool EnhanceFire();

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	void Pause();

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	void Resume();
};
