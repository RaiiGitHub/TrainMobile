// As Part of GuangZhou Training.

#pragma once

#include "GameFramework/Actor.h"
#include "SpriteActor.generated.h"

UCLASS()
class TRAINMOBILEMODEST_API ASpriteActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASpriteActor();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;
public:
	/**Sphere collision */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = SpriteActor)
	class USphereComponent *SphereCollision;
	/**Particle */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = SpriteActor)
	class UParticleSystemComponent *ParticleSystem;
	/**billboard for sprite*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = SpriteActor)
	class UBillboardComponent* SpriteBillboard;
private:
	FHitResult HitResult;
	class APawnHandler* PawnHandler;
	float EnsureParticlVisible;
public:
	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	bool HitOnSpriteTest();

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	FHitResult& GetHitResult();

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	void SpriteGetAwayPlease(const FString& tellMeSth);	//en..get away please

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	void SpriteComeOutPlease(const FString& tellMeSth);	//en..invoke the sprite come out,please

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	bool SetupSpriteRelativeLocation(const AActor* container);

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	void Reset();
};
