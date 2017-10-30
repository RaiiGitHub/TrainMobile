// As Part of GuangZhou Training.

#pragma once

#include "GameFramework/Character.h"
#include "HumanCharacter.generated.h"

UCLASS(config = Game)
class TRAINMOBILEMODEST_API AHumanCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

public:
	/**location on mesh where mesh object will be connected */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite,Category = MeshSlot)
	class USceneComponent* MeshSlot;

	/**mesh object attached to the mesh slot */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite,Category = MeshSlot)
	class UStaticMeshComponent* MeshStaticObject;

	/**Role Label */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Label)
	class URoleFollowLabelComponent* RoleLabel;

	/**Sprite Button*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Sprite)
	class USphereComponent *SpriteCollision1;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Sprite)
	class UBillboardComponent* Sprite1;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Sprite)
	class USphereComponent *SpriteCollision2;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Sprite)
	class UBillboardComponent* Sprite2;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Sprite)
	class USphereComponent *SpriteCollision3;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Sprite)
	class UBillboardComponent* Sprite3;

	/**Particle system*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Particle)
	class UParticleSystemComponent *ParticleSystem;

	/*Audio Component*/
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Particle)
	class UAudioComponent *ParticleAudio;

public:
	// Sets default values for this character's properties
	AHumanCharacter();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

public:
	//network-synchronize support
	class FSynchronizeSender
	{
	public:
		static void SyncToServer(const FString& pawn_name);
		static void SyncFromServer(const FString& pawn_name, const FString& ani_name, const FTransform &tf, const FVector& velocity);
	};
public:
	/**jump status */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	bool bJumpButtonDown;

	/**take on object */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	bool bTakeOnObject;

	/**whether can emit particles*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	bool bCanEmitParticle;

	/**whether can start to parepare open particle effect*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	bool bPrepareToOpenParticleEffect;

	// if necessary,for animation blueprint to determin the fix velocity(ignoring the block)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	float VelocityIgnoreCollision;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	float VelocityManualValueSend;//need to determin the stop status.

private:
	bool bParticleEffectOpen;
	AStaticMeshActor* HoldOnActor; // actor to be hold on,or took up.
	TMap<FString, bool> LogicKeys; //other logic keys.	
	void FixHandupPose(bool as_normal);
public:
	TMap<FString, bool>& GetLogicKeys();
	AStaticMeshActor* GetHoldOnMesh();
	void SetHoldOnMesh(AStaticMeshActor* mesh);
	void ParticlePress();
	void ParticleRelease();
	bool ParticleIsActive();
	bool SetActiveParticle(bool active);
	void JumpPress();
	void JumpRelease();
public:
	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	class APawnHandler* GetPawnHandler();

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	bool QueryLogicKey(const FString& key);

public:
	FRotator DefaultCameraRoator;//for reseting to the default.
	float DefaultArmLength;//for reseting arm length.
public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};
