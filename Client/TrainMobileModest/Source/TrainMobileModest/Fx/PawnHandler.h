// As Part of GuangZhou Training.

#pragma once

#include "GameFramework/Actor.h"
#include "PawnHandler.generated.h"


UENUM(BlueprintType)
enum ETouchEnableLevel
{	
	TEL_EnableMoveDisableClick,
	TEL_DisableClickNorMove,
	TEL_EnableClickAndMove
};

UCLASS()
class TRAINMOBILEMODEST_API APawnHandler : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	APawnHandler();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

private:
	APawn* currentHandlePawn_;
private:
	bool SetMeshComponentVisible(bool bVisible, USceneComponent* sc_root, const FString& mesh_name);
public:
	class URoleFollowLabelComponent* GetRoleLabel();
	class USpringArmComponent* GetRoleSpringArm();
	class UCameraComponent* GetRoleCamera();
public:
	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	void HandlePawn(APawn* pawn);

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	APawn* GetCurrentHandlePawn();

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	void FixCurrentRoleFirstSight(class ATaskPool* tp);

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	void AttachMeshToHandlePawn(AStaticMeshActor* sma);

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	void AttachMeshToSpecificPawn(APawn* pawn,AStaticMeshActor* sma);

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	void DetachMeshFromHandlePawn();

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	void DetachMeshFromSpecificPawn(APawn* pawn);

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	void SetBindMeshRelativeLocation(FVector location);

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	void SetBindMeshRelativeRotation(FRotator rotation);

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	void SetBindMeshRelativeScale(FVector scale);

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	bool MeshIsTakeOn();

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	bool BindBoneToSocket(const FName& socketName);

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	bool BindSpecificPawnBoneToSocket(const APawn* pawn,const FName& socketName);

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	bool GetHitResult(FHitResult& hr);

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	void Reset();
public:
	//sprite on character
	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	bool ActiveSprite(int spriteIndex, FString texPath);

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	bool IsSpriteActive(int spriteIndex);

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	bool InactiveSprite(int spriteIndex);

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	bool ActiveSpriteAgain(int spriteIndex);

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	bool HitOnSpriteTest(int spriteIndex);

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	bool HitOnSpriteTestAny();

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	FString GetSpriteTexPath(int spriteIndex);

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	bool ParticlePress();

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	bool ParticleRelease();

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	bool ParticleIsActive();

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	bool ParticleSpecificPawnPress(APawn* pawn);

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	bool ParticleSpecificPawnRelease(APawn* pawn);

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	bool ParticleSpecificPawnIsActive(APawn* pawn);

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	bool JumpPress();

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	bool JumpRelease();

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	bool JumpIsPress();

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	FVector GetPawnLocation();

public:
	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	bool SetLogicKey(const FString& key,bool val);

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	bool RemoveLogicKey(const FString& key);

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	bool QueryLogicKey(const FString& key);

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	bool EnableTouchLevel(ETouchEnableLevel tel = TEL_EnableClickAndMove);

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	bool SetDefaultFlyCamera();

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	bool ApplyDefaultFlyCamera();

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	bool SetAsDefaultFlyCameraRotation(const FRotator& r);

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	bool EnableAcceptControllerEvent(bool bEnable = true);

public:
	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	void ChangeScalarParameterOfCollection(const FString& collectionPath, FName paramName, float newVal);

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	void ChangeVectorParameterOfCollection(const FString& collectionPath, FName paramName, const FLinearColor& newVal);

};
