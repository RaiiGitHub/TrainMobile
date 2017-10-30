// As Part of GuangZhou Training.

#pragma once

#include "PawnHandler.h"
#include "GameFramework/PlayerController.h"
#include "ScenePlayerController.generated.h"

/**
 * 
 */
class APawnHandler;
class AActorTransformAni;
UCLASS()
class TRAINMOBILEMODEST_API AScenePlayerController : public APlayerController
{
	GENERATED_BODY()
protected:
	// Begin PlayerController interface
	virtual void PlayerTick(float DeltaTime) override;
	virtual void SetupInputComponent() override;
public:
	void MoveForward(float Value);
	void MoveRight(float Value);
	void ZoomCamera(float Value);
	void TurnAtRate(float Rate);
	void LookUpAtRate(float Rate);
	void SetDefaultCameraParam(const FRotator& r,const float l);
	void ResetDefaultCameraTag();
	void ApplyDefaultCameraParam();
protected:
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);
	void TouchRepeate(ETouchIndex::Type FingerIndex, FVector Location);
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);
protected:
	void SetNewMoveDestination(const FVector DestLocation);
	void SetNewMoveDestinationFree(const FVector DestLocation);
	void CheckReachHitPointAndNotify(const FVector DestLocation);
	void SetSmoothingCamera();
	void SetBodyRotateProcedure(const FVector DestLocation); 
	void SetFlyingDefaultCamera();
	void FinishFlyingDefaultCamera();
	void OnJumpPressed();
	void OnJumpReleased();
	void OnJumpRepeate();
protected:
	void ResetInputParams();
	void NotifyHitResult(FHitResult* hr = nullptr);
public:
	const FHitResult& GetHitResult() const;
	void ClearHitResult();
	void CleanHitEffect(APawn* prePawn, bool affectDir = true);
	void EnableTouchLevel(ETouchEnableLevel tel = TEL_EnableClickAndMove);
	void EnableTick(bool enable);
	void SetupDefaultFlyingCameraRotation();
private:
	enum {
		BODY_ROTATE_IDLE,
		BODY_ROTATE_ACTING,
		BODY_ROTATE_DONE
	}BodyRotataState = BODY_ROTATE_IDLE;
	enum {
		BODY_MOVEMENT_IDLE,
		BODY_MOVEMENT_ACTING
	}BodyMovementState = BODY_MOVEMENT_IDLE;
	enum {
		CAMERA_DEFAULT_IDLE,
		CAMERA_DEFAULT_ACTING,
		CAEMRA_DEFAULT_DONE
	}CameraDefaultState = CAMERA_DEFAULT_IDLE;
private:
	APawnHandler* PawnHandler = nullptr;
	AActorTransformAni* TransformAni = nullptr;
	FHitResult HitResult;
	float PinchLastValue = 0.f;
	FVector2D RotateCamera = FVector2D::ZeroVector;
	float DragLength = 0.f;
	float TurnOrLookUpLength = 0.f;
	FVector2D Touch1Location = FVector2D::ZeroVector;
	FVector2D Touch2Location = FVector2D::ZeroVector;
	bool bResetDefaultCamera = false;
	ETouchEnableLevel TouchEnableLv = TEL_EnableClickAndMove;
	bool TouchDistableReady = false;
	//flying to default camera.
	FRotator FlyBeginRotation;
	FRotator FlyEndRotation;
	float    FlyDefaultTargetArmLength;
	float    FlyBeginArmLength;
	float    FlyBeginTime;
public:
	static AScenePlayerController* Handler;
};
