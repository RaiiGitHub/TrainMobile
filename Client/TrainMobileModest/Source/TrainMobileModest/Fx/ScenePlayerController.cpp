// As Part of GuangZhou Training.

#include "TrainMobileModest.h"
#include "ScenePlayerController.h"
#include "RoleFollowLabelComponent.h"
#include "CallbackActor.h"
#include "ActorTransformAni.h"
#include "TaskPool.h"
#include "Engine/Canvas.h"
#include "DisplayDebugHelpers.h"
#include "TrainGameInstance.h"
#include "HumanCharacter.h"

static const float MovementScale = .618f;
static const float ArmLengthBench = 280.f;
static const float BaseTurnRate = 15.f;
static const float BaseLookUpRate = 15.f;

static float PawnRotateYawTime = 0.f;
static FQuat PawnRotateQuat = FQuat::Identity;
static FQuat PawnRotateQuatFinal = FQuat::Identity;
static float PawnRotateBeingTime = 0.f;

static FVector PawnCam = FVector::ZeroVector;
static FVector PawnCamDir = FVector::ZeroVector;
static float   PawnFlyDis = 0.f;

static FQuat CamSmoothBeginQuat = FQuat::Identity;
static FQuat CamSmoothEndQuat = FQuat::Identity;
static float CamSmoothBeginTime = 0.f;
static float CamSmoothBeginSpeed = 0.f;
static float CamSmoothCurrentSpeed = 0.f;
static float CamSmoothAcceleration = 0.f;

DECLARE_LOG_CATEGORY_CLASS(RaiiLog, Log, All);

AScenePlayerController* AScenePlayerController::Handler = nullptr;
void AScenePlayerController::PlayerTick(float DeltaTime)
{
	//keep updating the destination every tick while desired
	Super::PlayerTick(DeltaTime);
	if (!PrimaryActorTick.bCanEverTick)
	{
		ClearHitResult();
		SetSmoothingCamera();
		SetFlyingDefaultCamera();
		if (PawnHandler && PawnHandler->GetCurrentHandlePawn())
		{
			AHumanCharacter* pawn = static_cast<AHumanCharacter*>(PawnHandler->GetCurrentHandlePawn());
			if (0.f == pawn->GetVelocity().Size()
				&& 0.f != pawn->VelocityManualValueSend)
			{
				AHumanCharacter::FSynchronizeSender::SyncToServer(pawn->GetName());
			}
		}
		return;
	}
	if (HitResult.bBlockingHit)
	{
		if (PawnHandler && PawnHandler->GetCurrentHandlePawn())
		{
			//hit something,move there.
			if (BODY_ROTATE_DONE != BodyRotataState)
				SetBodyRotateProcedure(HitResult.ImpactPoint);
			else
				CheckReachHitPointAndNotify(HitResult.ImpactPoint);//SetNewMoveDestination(HitResult.ImpactPoint);
		}
		else
		{
			SetNewMoveDestinationFree(HitResult.ImpactPoint);
		}
	}
	else
	{
		SetSmoothingCamera();
		SetFlyingDefaultCamera();
	}
	//
	if (PawnHandler && PawnHandler->GetCurrentHandlePawn())
	{
		AHumanCharacter* pawn = static_cast<AHumanCharacter*>(PawnHandler->GetCurrentHandlePawn());
		if (0.f == pawn->GetVelocity().Size() 
			&& 0.f != pawn->VelocityManualValueSend)
		{
			AHumanCharacter::FSynchronizeSender::SyncToServer(pawn->GetName());
		}
	}
	//OutputDebugString(*FString::Printf(TEXT("drag_len:%f\n"), DragLength));
}

void AScenePlayerController::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();

	UE_LOG(RaiiLog, Log, TEXT("AScenePlayerController::SetupInputComponent called."));
	InputComponent->BindAction("Jump", IE_Pressed, this, &AScenePlayerController::OnJumpPressed);
	InputComponent->BindAction("Jump", IE_Repeat, this, &AScenePlayerController::OnJumpRepeate);
	InputComponent->BindAction("Jump", IE_Released, this, &AScenePlayerController::OnJumpReleased);

	InputComponent->BindAxis("MoveForward", this, &AScenePlayerController::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &AScenePlayerController::MoveRight);

	InputComponent->BindAxis("ZoomCamera", this, &AScenePlayerController::ZoomCamera);
	InputComponent->BindAxis("TurnRate", this, &AScenePlayerController::TurnAtRate);
	InputComponent->BindAxis("LookUpRate", this, &AScenePlayerController::LookUpAtRate);

	// handle touch devices
	InputComponent->BindTouch(IE_Pressed, this, &AScenePlayerController::TouchStarted);
	InputComponent->BindTouch(IE_Repeat, this, &AScenePlayerController::TouchRepeate);
	InputComponent->BindTouch(IE_Released, this, &AScenePlayerController::TouchStopped);

	//pawn handler
	PawnHandler = FindObject<APawnHandler>(ANY_PACKAGE, TEXT("PawnHandler"));
	TransformAni = FindObject<AActorTransformAni>(ANY_PACKAGE, TEXT("ActorTransformAni"));

	//handler instance
	Handler = this;
	EnableTick(false);
}

void AScenePlayerController::MoveForward(float Value)
{
	if (PrimaryActorTick.bCanEverTick && Value != 0.0f)
	{
		// find out which way is forward
		const FRotator Rotation = GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		DragLength = 0.f;
		if (PawnHandler)
		{
			APawn* pawn = PawnHandler->GetCurrentHandlePawn();
			if (nullptr == pawn)
			{
				//as default pawn.
				ADefaultPawn* dp = (ADefaultPawn*)GetPawn();
				dp->MoveForward(Value*MovementScale);
			}
			else
			{
				pawn->AddMovementInput(Direction, Value*MovementScale);
				//sync to server
				if (UTrainGameInstance::Instance
					&& ETrainMode::TrainMode_Multi_Online_Player == UTrainGameInstance::Instance->GetTrainMode())
				{
					AHumanCharacter::FSynchronizeSender::SyncToServer(pawn->GetName());
				}
			}
		}
		CAMERA_DEFAULT_ACTING == CameraDefaultState ?
			FinishFlyingDefaultCamera() : SetupDefaultFlyingCameraRotation();
	}
}

void AScenePlayerController::MoveRight(float Value)
{
	if (PrimaryActorTick.bCanEverTick && Value != 0.0f)
	{
		// find out which way is right
		const FRotator Rotation = GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		DragLength = 0.f;
		if (PawnHandler)
		{
			APawn* pawn = PawnHandler->GetCurrentHandlePawn();
			if (nullptr == pawn)
			{
				//as default pawn.
				ADefaultPawn* dp = (ADefaultPawn*)GetPawn();
				dp->MoveRight(Value*MovementScale);
			}
			else
			{
				pawn->AddMovementInput(Direction, Value*MovementScale);
				//sync to server
				if (UTrainGameInstance::Instance
					&& ETrainMode::TrainMode_Multi_Online_Player == UTrainGameInstance::Instance->GetTrainMode())
				{
					AHumanCharacter::FSynchronizeSender::SyncToServer(pawn->GetName());
				}
			}
		}
		CAMERA_DEFAULT_ACTING == CameraDefaultState ?
			FinishFlyingDefaultCamera() : SetupDefaultFlyingCameraRotation();
	}
}

void AScenePlayerController::ZoomCamera(float Value)
{
	if (PrimaryActorTick.bCanEverTick && Value != 0.0f)
	{
		DragLength += FMath::Abs(Value);
		static const float limit_percent = .8f;
		if (PawnHandler && PawnHandler->GetCurrentHandlePawn() )
		{
			USpringArmComponent* camera_boom = PawnHandler->GetRoleSpringArm();
			UCameraComponent* follow_camera = PawnHandler->GetRoleCamera();
			if (nullptr == camera_boom || nullptr == follow_camera)
				return;
			camera_boom->TargetArmLength += Value;
			camera_boom->TargetArmLength = FMath::Max(camera_boom->TargetArmLength, ArmLengthBench*(1 - limit_percent));
			camera_boom->TargetArmLength = FMath::Min(camera_boom->TargetArmLength, ArmLengthBench*(1 + limit_percent));
			follow_camera->FieldOfView = FMath::Lerp<float>(90.0f, 45.f,
				(ArmLengthBench - camera_boom->TargetArmLength) / ArmLengthBench);
		}
		else
		{
			FVector fv = GetActorForwardVector();
			FVector uv = GetActorUpVector();
 			PawnCam = GetPawn()->GetActorLocation();
			FVector dir = -fv + uv;
			PawnCamDir = dir * Value/FMath::Abs(Value);
			HitResult.bBlockingHit = true;
#if PLATFORM_ANDROID
			PawnFlyDis = FMath::Abs(Value);
#else
			PawnFlyDis = 5000.f;
#endif	
			RotateCamera = FVector2D::ZeroVector;
		}
		if (CAMERA_DEFAULT_ACTING == CameraDefaultState)
			FinishFlyingDefaultCamera();
	}
}

void AScenePlayerController::TurnAtRate(float Rate)
{
	if (PrimaryActorTick.bCanEverTick && Rate != 0.0f)
	{
		Rate = MovementScale * Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds();
		TurnOrLookUpLength += FMath::Abs(Rate);
		// calculate delta for this frame from the rate information
		if (PawnHandler)
		{
			APawn* pawn = PawnHandler->GetCurrentHandlePawn();
			if (nullptr == pawn)
			{
				//as default pawn.
				ADefaultPawn* dp = (ADefaultPawn*)GetPawn();
				dp->TurnAtRate(Rate);
			}
			else
			{
				pawn->AddControllerYawInput(Rate);
			}
		}
		if (CAMERA_DEFAULT_ACTING == CameraDefaultState)
			FinishFlyingDefaultCamera();
	}
}

void AScenePlayerController::LookUpAtRate(float Rate)
{
	if (PrimaryActorTick.bCanEverTick && Rate != 0.0f)
	{
		Rate = MovementScale * Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds();
		TurnOrLookUpLength += FMath::Abs(Rate);
		if (PawnHandler)
		{
			APawn* pawn = PawnHandler->GetCurrentHandlePawn();
			if (nullptr == pawn)
			{
				//as default pawn.
				ADefaultPawn* dp = (ADefaultPawn*)GetPawn();
				dp->LookUpAtRate(Rate);
			}
			else
			{
				FRotator r = pawn->GetControlRotation();
				r.Pitch += Rate * InputPitchScale;
				FQuat q(r);
				if (q.Euler().Y <= 20.f && q.Euler().Y >= -40.f)
					pawn->AddControllerPitchInput(Rate);
				else if (q.Euler().Y > 20.f)
				{
					r.Pitch = 20.f;
					pawn->GetController()->SetControlRotation(r);
				}
				else if (q.Euler().Y < -40.f)
				{
					r.Pitch = -40.f;
					pawn->GetController()->SetControlRotation(r);
				}
				
				//URoleFollowLabelComponent* role_label = PawnHandler->GetRoleLabel();
				//if (role_label)
				//	role_label->UpdateRoleLabel();
			}
			//FRotator r = GetPawn()->GetControlRotation();
			//OutputDebugString(*FString::Printf(TEXT("Rate->%f,r:%f,p:%f,y:%f\n"), Rate, r.Roll, r.Pitch, r.Yaw));
		}
		if (CAMERA_DEFAULT_ACTING == CameraDefaultState)
			FinishFlyingDefaultCamera();
	}
}

void AScenePlayerController::SetDefaultCameraParam(const FRotator& r, const float l)
{
	FlyDefaultTargetArmLength = l;
	FlyEndRotation = r;
	FlyEndRotation.Pitch = -5.f;
}

void AScenePlayerController::ResetDefaultCameraTag()
{
	bResetDefaultCamera = false;
}

void AScenePlayerController::ApplyDefaultCameraParam()
{
	//begin to fly to the default camera.
	if (nullptr == PawnHandler)
		return;
	USpringArmComponent* camera_boom = PawnHandler->GetRoleSpringArm();
	if (nullptr == camera_boom)
		return;
	FMath::RandInit(666);
	CameraDefaultState = CAMERA_DEFAULT_ACTING;
	FlyBeginArmLength = camera_boom->TargetArmLength*(1.f + FMath::RandRange(-0.1f,0.1f));
	FlyBeginRotation = GetPawn()->GetControlRotation();
	FlyBeginTime = GetWorld()->GetTimeSeconds();
}

void AScenePlayerController::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
	if (TEL_DisableClickNorMove == TouchEnableLv )
		return;
	ResetInputParams();
	//for smoothing.
	if (PawnHandler)
	{
		APawn* pawn = PawnHandler->GetCurrentHandlePawn();
		if (pawn)
		{
			CamSmoothBeginQuat = FQuat(pawn->GetControlRotation());
			CamSmoothBeginTime = GetWorld()->GetTimeSeconds();
			CamSmoothEndQuat = FQuat::Identity;
			CamSmoothBeginSpeed = 0.f;
			CamSmoothCurrentSpeed = 0.f;
			CamSmoothAcceleration = 0.f;
		}
	}
}

void AScenePlayerController::TouchRepeate(ETouchIndex::Type FingerIndex, FVector Location)
{
	if (TEL_DisableClickNorMove == TouchEnableLv)
		return;
	//zoom
	FVector2D screen_pos_1 = FVector2D::ZeroVector;
	FVector2D screen_pos_2 = FVector2D::ZeroVector;
	FVector2D screen_pos_3 = FVector2D::ZeroVector;
	FVector2D screen_pos_4 = FVector2D::ZeroVector;
	bool pressed_1 = false;
	bool pressed_2 = false;
	bool pressed_3 = false;
	bool pressed_4 = false;
	GetInputTouchState(ETouchIndex::Touch1, screen_pos_1.X, screen_pos_1.Y, pressed_1);
	GetInputTouchState(ETouchIndex::Touch2, screen_pos_2.X, screen_pos_2.Y, pressed_2);
	GetInputTouchState(ETouchIndex::Touch3, screen_pos_3.X, screen_pos_3.Y, pressed_3);
	GetInputTouchState(ETouchIndex::Touch4, screen_pos_4.X, screen_pos_4.Y, pressed_4);

	if (pressed_3 || pressed_4)
	{
		DragLength = 1.f;
		return;
	}

	if ( pressed_1 && pressed_2)
	{
		//two fingers.
		if (Touch1Location.IsZero() || Touch2Location.IsZero())
		{
			Touch1Location = screen_pos_1;
			Touch2Location = screen_pos_2;
		}
		float cur_len = FVector2D::Distance(screen_pos_2, screen_pos_1);
		float pre_len = FVector2D::Distance(Touch2Location, Touch1Location);
		ZoomCamera( pre_len - cur_len);
		Touch1Location = screen_pos_1;
		Touch2Location = screen_pos_2;
		RotateCamera = FVector2D::ZeroVector;
	}
	else if (FMath::IsNearlyEqual(DragLength, 0.f, 0.618f))
	{
		//one finger,here will be 2 conditions.
		//c1: one finger all the time,drag the camera. here the drag length will always be zero.
		//c2: two fingers at first,then release one,but the drag length was not zero.
		PawnRotateYawTime = 0.f;
		PawnRotateQuat = FQuat::Identity;
		PawnRotateQuatFinal = FQuat::Identity;
		PawnRotateBeingTime = 0.f;

		PawnCam = FVector::ZeroVector;
		PawnCamDir = FVector::ZeroVector;
		PawnFlyDis = 0.f;

		FVector2D ScreenSpaceLocation(Location);
		//to see left or right
		if (RotateCamera.IsZero())
			RotateCamera = ScreenSpaceLocation;
		float turn = ScreenSpaceLocation.X - RotateCamera.X;
		float look_up = ScreenSpaceLocation.Y - RotateCamera.Y;

#if PLATFORM_ANDROID
		turn *= 0.318;
		look_up *= 0.318;
#else
		turn *= 0.318;
		look_up *= 0.318;
#endif
		TurnAtRate(turn);
		LookUpAtRate(look_up);
		RotateCamera = ScreenSpaceLocation;
	}
	else
		return;

	//
	if (PawnHandler)
	{
		if (PawnHandler->GetCurrentHandlePawn())
		{
			if (!bResetDefaultCamera)
			{
				static ACallbackActor* callbackActor = FindObject<ACallbackActor>(ANY_PACKAGE, TEXT("CallbackActor"));
				if (callbackActor)
				{
					callbackActor->SetCallbackType(Callback_NotifyCameraFlyNeedToReset);
					callbackActor->NotifyCameraFlyNeedToReset();
					bResetDefaultCamera = true;
				}
			}
		}
	}
}

void AScenePlayerController::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
	if (TEL_DisableClickNorMove == TouchEnableLv)
		return;
	if(FMath::IsNearlyEqual(DragLength,0.f,0.618f))
	{
		if (FMath::IsNearlyEqual(TurnOrLookUpLength, 0.f, 0.618f))
		{
			if (TEL_EnableClickAndMove == TouchEnableLv )
			{
				//touch,collect target location and prepare to trigger the effect of hitten objects.
				FVector2D ScreenSpaceLocation(Location);
				GetHitResultAtScreenPosition(ScreenSpaceLocation, CurrentClickTraceChannel, true, HitResult);
				PawnRotateYawTime = 0.f;
				PawnRotateQuat = FQuat::Identity;
				PawnRotateQuatFinal = FQuat::Identity;
				PawnRotateBeingTime = 0.f;
				BodyRotataState = BODY_ROTATE_IDLE;

				PawnCam = FVector::ZeroVector;
				PawnCamDir = FVector::ZeroVector;
				PawnFlyDis = 0.f;

				//if (PawnHandler && PawnHandler->HitOnSpriteTestAny())
				//{
				//	//do not show pointer while hit on the components of the character.
				//	NotifyHitResult();
				//	ClearHitResult();
				//}
				//else
				//{
				//	//show location pointer.
				//}
			}
		}
		else
		{
			//turn or look up the camera.
			APawn* const pawn = GetPawn();
			if (pawn)
			{
				CamSmoothEndQuat = FQuat(pawn->GetControlRotation());
				float angularDis = CamSmoothEndQuat.AngularDistance(CamSmoothBeginQuat);
				float elapse_time = GetWorld()->GetTimeSeconds() - CamSmoothBeginTime;
				CamSmoothBeginSpeed = angularDis / elapse_time;
				//v will from speed to 0,in a const acceleration.
				CamSmoothAcceleration = -CamSmoothBeginSpeed / .618f;//smoothing within few seconds.
				CamSmoothCurrentSpeed = CamSmoothBeginSpeed;
			}
			ClearHitResult();
		}
	}
}

void AScenePlayerController::SetNewMoveDestination(const FVector DestLocation)
{
	APawn* const pawn = GetPawn();
	if (pawn)
	{
		UNavigationSystem* const NavSys = GetWorld()->GetNavigationSystem();
		//height should not be calculated.
		float const Distance = FVector::Dist(FVector(FVector2D(DestLocation),0.f),
			FVector(FVector2D(pawn->GetActorLocation()),0.f));
		// We need to issue move command only if far enough in order for walk animation to play correctly
		if (NavSys)
		{
			FVector md = FVector::ZeroVector;
			float   ml = 0.f;
			pawn->GetVelocity().ToDirectionAndLength(md, ml);
			bool    stop = FMath::IsNearlyZero(ml, .00001f);
			if (!stop && BODY_MOVEMENT_IDLE == BodyMovementState)
				BodyMovementState = BODY_MOVEMENT_ACTING;
			if (Distance > 120.0f)
			{
				PawnRotateYawTime = 0.f;
				PawnRotateBeingTime = 0.f;
				NavSys->SimpleMoveToLocation(this, DestLocation);
				//may block by the obstacles.
				if (stop && BODY_MOVEMENT_ACTING == BodyMovementState )
					ClearHitResult();
				//sync to server
				if (UTrainGameInstance::Instance
					&& ETrainMode::TrainMode_Multi_Online_Player == UTrainGameInstance::Instance->GetTrainMode())
				{
					AHumanCharacter::FSynchronizeSender::SyncToServer(GetPawn()->GetName());
				}
				CAMERA_DEFAULT_ACTING == CameraDefaultState ?
					FinishFlyingDefaultCamera() : SetupDefaultFlyingCameraRotation();
			}
			else
			{
				AStaticMeshActor* mesh = TransformAni->GetPointerMesh();
				if (mesh && mesh->bHidden)
					NotifyHitResult();
				ClearHitResult();
			}	
		}
		//if (PawnHandler)
		//{
		//	URoleFollowLabelComponent* role_label = PawnHandler->GetRoleLabel();
		//	if (role_label)
		//		role_label->UpdateRoleLabel();
		//}
	}
}

void AScenePlayerController::SetNewMoveDestinationFree(const FVector DestLocation)
{
	APawn* const pawn = GetPawn();
	if (pawn)
	{
		if (0.f == PawnFlyDis)
		{
			const FVector pos = pawn->GetActorLocation();
			(DestLocation - pos).ToDirectionAndLength(PawnCamDir, PawnFlyDis);
			PawnCam = pos;
			if (FMath::IsNearlyZero(PawnFlyDis))
			{
				PawnCam = FVector::ZeroVector;
				PawnCamDir = FVector::ZeroVector;
				PawnFlyDis = 0.f;
				ClearHitResult();
				return;
			}
		}
		FVector cur_pos = pawn->GetActorLocation();
		float cur_PawnFlyDis = FVector::Dist(cur_pos, PawnCam);
		float cur_fly_offset_line = FMath::PointDistToLine(cur_pos, PawnCamDir, PawnCam);
		//to check whether complete flying or not,as well as whether deviate or not.
		if (cur_PawnFlyDis >= PawnFlyDis || cur_fly_offset_line > 50.f)
		{
			PawnCam = FVector::ZeroVector;
			PawnCamDir = FVector::ZeroVector;
			PawnFlyDis = 0.f;
			ClearHitResult();
			return;
		}
		pawn->AddMovementInput(PawnCamDir);
	}
}

void AScenePlayerController::CheckReachHitPointAndNotify(const FVector DestLocation)
{
	APawn* const pawn = GetPawn();
	if (pawn)
	{
		float const Distance = FVector::Dist(FVector(FVector2D(DestLocation), 0.f),
			FVector(FVector2D(pawn->GetActorLocation()), 0.f));
		if (Distance > 240.0f)
		{

		}
		else
		{
			AStaticMeshActor* mesh = TransformAni->GetPointerMesh();
			if (mesh && mesh->bHidden)
			{
				NotifyHitResult();
			}	
			else
			{
				NotifyHitResult(&HitResult);
			}
			ClearHitResult();
		}
	}
}

void AScenePlayerController::SetSmoothingCamera()
{
	if (0.f == CamSmoothCurrentSpeed)
		return;
	APawn* const pawn = GetPawn();
	if (pawn)
	{
		const float angle_dis = CamSmoothEndQuat.AngularDistance(CamSmoothBeginQuat);
		const float delta_time = GetWorld()->GetDeltaSeconds ();
		CamSmoothCurrentSpeed += CamSmoothAcceleration * delta_time;//new speed.
		const float cur_elapse_time = (CamSmoothCurrentSpeed - CamSmoothBeginSpeed)/CamSmoothAcceleration;
		const float moving_dis = CamSmoothBeginSpeed * cur_elapse_time + .5f * CamSmoothAcceleration * cur_elapse_time * cur_elapse_time;
		const float moving_percent = moving_dis / angle_dis;
		const FQuat move_quat(FQuat::Slerp(CamSmoothBeginQuat, CamSmoothEndQuat, 1.f + moving_percent));
		CamSmoothCurrentSpeed = FMath::Max(0.f, CamSmoothCurrentSpeed);
		FRotator move_rotation(move_quat);
		move_rotation.Roll = 0.f;
		move_rotation.Pitch = FMath::Min(20.f, move_rotation.Pitch);
		move_rotation.Pitch = FMath::Max(-40.f, move_rotation.Pitch);
		pawn->GetController()->SetControlRotation(move_rotation);
		//if (PawnHandler)
		//{
		//	URoleFollowLabelComponent* role_label = PawnHandler->GetRoleLabel();
		//	if (role_label)
		//		role_label->UpdateRoleLabel();
		//}
		//GEngine->AddOnScreenDebugMessage(
		//	-1,
		//	2.f,
		//	FColor::Red,
		//	FString::Printf(TEXT("%f:smooth data ~> Yaw:%f,Pitch:%f; percent:%f"),
		//		GetWorld()->GetTimeSeconds(),
		//		move_rotation.Yaw, move_rotation.Pitch, moving_percent));
	}
}

void AScenePlayerController::SetBodyRotateProcedure(const FVector DestLocation)
{
    //rotate body first.
	APawn* const pawn = GetPawn();
	if (nullptr == pawn)
		return;
	if (BODY_ROTATE_IDLE == BodyRotataState)
	{
		const FVector pos = pawn->GetActorLocation();
		const FVector dir = DestLocation - pos;
		FVector dirNormal = FVector::ZeroVector;
		float dirLen = 0.f;
		dir.ToDirectionAndLength(dirNormal, dirLen);
		const FRotator rotate = dirNormal.ToOrientationRotator();
		const FRotator rotate_p = pawn->GetActorRotation();
		PawnRotateQuat = rotate_p.Quaternion();
		PawnRotateQuatFinal = FRotator(rotate_p.Pitch, rotate.Yaw, rotate_p.Roll).Quaternion();
		PawnRotateBeingTime = GetWorld()->GetTimeSeconds();
		BodyRotataState = BODY_ROTATE_ACTING;
	}
	if (BODY_ROTATE_ACTING == BodyRotataState)
	{
		//not rotate at an average rate.
		static const float v_as = 3.15f;//degree/s
		const float  ad = PawnRotateQuatFinal.AngularDistance(PawnRotateQuat);
		PawnRotateYawTime += GetWorld()->GetTimeSeconds() - PawnRotateBeingTime;
		const float ad_now = v_as * PawnRotateYawTime;
		FRotator cur_r = pawn->GetActorRotation();
		if (ad_now >= ad)
		{
			pawn->SetActorRotation(PawnRotateQuatFinal);
			PawnRotateYawTime = 0.f;
			PawnRotateQuat = FQuat::Identity;
			PawnRotateQuatFinal = FQuat::Identity;
			PawnRotateBeingTime = 0.f;
			BodyRotataState = BODY_ROTATE_DONE;
		}
		else
		{
			const FQuat lerp_quat = FMath::Lerp(PawnRotateQuat, PawnRotateQuatFinal, ad_now/ad);
			pawn->SetActorRotation(lerp_quat);
			//sync to server
			if (UTrainGameInstance::Instance
				&& ETrainMode::TrainMode_Multi_Online_Player == UTrainGameInstance::Instance->GetTrainMode())
			{
				AHumanCharacter::FSynchronizeSender::SyncToServer(pawn->GetName());
			}
		}
	}
}

void AScenePlayerController::SetFlyingDefaultCamera()
{
	if (PawnHandler && CAMERA_DEFAULT_ACTING == CameraDefaultState)
	{
		//acting.
		//the target rotation should be auto changed.
		APawn* const pawn = GetPawn();
		//const FRotator target_r = pawn->GetActorRotation();
		const FRotator target_r = FlyEndRotation;
		FQuat br(FlyBeginRotation);
		FQuat dr(target_r);
		static const float v_a = 1.5f;//degree/s.
		static const float v_l = 314.f;//cm/s.
		const float angle_dis = br.AngularDistance(dr);
		const float delta_time = GetWorld()->GetTimeSeconds() - FlyBeginTime;
		const float cur_angle = v_a * delta_time;
		const float primitive_percent = cur_angle / angle_dis;
		const float arm_dis_percent = v_l* delta_time / FlyBeginArmLength;
		float moving_percent = FMath::Min(arm_dis_percent, primitive_percent);
		const float min_y = 0.005f;
		const float max_x = -FMath::Loge(min_y);
		moving_percent = FMath::Min(1.f, moving_percent);
		moving_percent = FMath::Max(min_y, moving_percent);
		//y = e-x
		//-x = ln(y)
		moving_percent = (max_x + FMath::Loge(moving_percent)) / max_x;
		moving_percent = FMath::Min(1.f, moving_percent);
		
		const float arm_len(FMath::Lerp<float>(FlyBeginArmLength, FlyDefaultTargetArmLength,moving_percent));
		const FQuat move_quat(FQuat::Slerp(br, dr, moving_percent));
		FRotator move_rotation(move_quat);
		move_rotation.Roll = 0.f;
		GetPawn()->GetController()->SetControlRotation(move_rotation);
		USpringArmComponent* camera_boom = PawnHandler->GetRoleSpringArm();
		UCameraComponent* follow_camera = PawnHandler->GetRoleCamera();
		if ( camera_boom && follow_camera )
		{
			camera_boom->TargetArmLength = arm_len;
			follow_camera->FieldOfView = FMath::Lerp<float>(90.0f, 45.f,
				(ArmLengthBench - camera_boom->TargetArmLength) / ArmLengthBench);
		}
		//
		//if (PawnHandler)
		//{
		//	URoleFollowLabelComponent* role_label = PawnHandler->GetRoleLabel();
		//	if (role_label)
		//		role_label->UpdateRoleLabel();
		//}
		//
		if (moving_percent == 1.f)
			FinishFlyingDefaultCamera();
	}
}

void AScenePlayerController::SetupDefaultFlyingCameraRotation()
{
	//calculate end rotation.
	FlyEndRotation = GetPawn()->GetControlRotation();
	FlyEndRotation.Pitch = -5.f;
}

void AScenePlayerController::FinishFlyingDefaultCamera()
{
	CameraDefaultState = CAEMRA_DEFAULT_DONE;
	ACallbackActor* callbackActor = FindObject<ACallbackActor>(ANY_PACKAGE, TEXT("CallbackActor"));
	if (callbackActor)
	{
		callbackActor->SetCallbackType(Callback_NotifyCameraFlyToDefault);
		callbackActor->NotifyCameraFlyToDefaultDone();
	}
	bResetDefaultCamera = false;
}

void AScenePlayerController::OnJumpPressed()
{
	if (nullptr == PawnHandler)
		PawnHandler = FindObject<APawnHandler>(ANY_PACKAGE, TEXT("PawnHandler"));
	if (PawnHandler)
	{
		PawnHandler->JumpPress();
		//sync to server
		if (UTrainGameInstance::Instance
			&& ETrainMode::TrainMode_Multi_Online_Player == UTrainGameInstance::Instance->GetTrainMode())
		{
			AHumanCharacter::FSynchronizeSender::SyncToServer(GetPawn()->GetName());
		}
	}
}

void AScenePlayerController::OnJumpReleased()
{
	if (PawnHandler)
	{
		PawnHandler->JumpRelease();
		//sync to server
		if (UTrainGameInstance::Instance
			&& ETrainMode::TrainMode_Multi_Online_Player == UTrainGameInstance::Instance->GetTrainMode())
		{
			AHumanCharacter::FSynchronizeSender::SyncToServer(GetPawn()->GetName());
		}
	}
}

void AScenePlayerController::OnJumpRepeate()
{
	if (PawnHandler)
	{
		if (!PawnHandler->JumpIsPress())
			PawnHandler->JumpPress();
		else
		{
			//sync to server
			if (UTrainGameInstance::Instance
				&& ETrainMode::TrainMode_Multi_Online_Player == UTrainGameInstance::Instance->GetTrainMode())
			{
				AHumanCharacter::FSynchronizeSender::SyncToServer(GetPawn()->GetName());
			}
		}
	}
}

void AScenePlayerController::ResetInputParams()
{
	RotateCamera = FVector2D::ZeroVector;
	Touch1Location = FVector2D::ZeroVector;
	Touch2Location = FVector2D::ZeroVector;
	RotateCamera = FVector2D::ZeroVector;
	DragLength = 0.f;
	TurnOrLookUpLength = 0.f;
}

void AScenePlayerController::NotifyHitResult(FHitResult* hr)
{
	ACallbackActor* callbackActor = FindObject<ACallbackActor>(ANY_PACKAGE, TEXT("CallbackActor"));
	if (callbackActor)
	{
		callbackActor->SetCallbackType(Callback_NotifyHitResult);
		hr?callbackActor->NotifyHitResultFree(*hr):callbackActor->NotifyHitResult();
	}
}

void AScenePlayerController::ClearHitResult()
{
	HitResult.Reset();
	StopMovement();
	BodyRotataState = BODY_ROTATE_IDLE;
	BodyMovementState = BODY_MOVEMENT_IDLE;
	APawn* const pawn = GetPawn();
	if (pawn)
		pawn->ConsumeMovementInputVector();	
}

void AScenePlayerController::CleanHitEffect(APawn* prePawn, bool affectDir)
{
	if (affectDir)
	{
		if (prePawn && prePawn->GetMovementComponent())
			prePawn->GetMovementComponent()->Velocity = FVector::ZeroVector;
		UNavigationSystem* const NavSys = GetWorld()->GetNavigationSystem();
		if (NavSys)
			NavSys->SimpleMoveToActor(this, GetPawn());
	}
}

void AScenePlayerController::EnableTouchLevel(ETouchEnableLevel tel)
{
	TouchEnableLv = tel;
	ClearHitResult();
	//TouchDistableReady |= (tel == TEL_DisableClickNorMove);
	//GEngine->AddOnScreenDebugMessage(
	//	-1,
	//	5.f,
	//	FColor::Red,
	//	FString::Printf(TEXT("EnableTouchLevel:%s"), 
	//		TEL_EnableMoveDisableClick == tel? TEXT("TEL_EnableMoveDisableClick")
	//		:(TEL_DisableClickNorMove == tel? TEXT("TEL_DisableClickNorMove")
	//		:TEXT("TEL_EnableClickAndMove"))));
}

void AScenePlayerController::EnableTick(bool enable)
{
	PrimaryActorTick.bCanEverTick = enable;
	ClearHitResult();
	//GEngine->AddOnScreenDebugMessage(
	//	-1,
	//	5.f,
	//	FColor::Red,
	//	FString::Printf(TEXT("EnableTick:%s"), 
	//		enable? TEXT("true")
	//		:TEXT("false")));
}

const FHitResult & AScenePlayerController::GetHitResult() const
{
	// TODO: insert return statement here
	return HitResult;
}



