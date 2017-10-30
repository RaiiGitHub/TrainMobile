// As Part of GuangZhou Training.

#include "TrainMobileModest.h"
#include "PawnHandler.h"
#include "HumanCharacter.h"
#include "TextureFinder.h"
#include "TaskPool.h"
#include "RoleFollowLabelComponent.h"
#include "ScenePlayerController.h"
#include "Engine/StaticMeshActor.h"


// Sets default values
APawnHandler::APawnHandler()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to  improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	currentHandlePawn_ = NULL;
}

// Called when the game starts or when spawned
void APawnHandler::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APawnHandler::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

bool APawnHandler::SetMeshComponentVisible(bool bVisible, USceneComponent * sc_root, const FString & mesh_name)
{
	if (nullptr == sc_root )
		return false;
	for (auto& isc : sc_root->GetAttachChildren())
	{
		if (isc->GetName().Equals(mesh_name))
		{
			isc->SetHiddenInGame(!bVisible);
			return true;
		}
		if (SetMeshComponentVisible(bVisible, isc, mesh_name))
			return true;
	}
	return false;
}

void APawnHandler::FixCurrentRoleFirstSight(ATaskPool* tp)
{
	if (nullptr == currentHandlePawn_ || nullptr == tp )
		return;
	FRoleFirstSight rfs = tp->GetCurrentRoleFirstSight();
	if (rfs.bEnable)
	{
		AScenePlayerController* pc = static_cast<AScenePlayerController*>(currentHandlePawn_->GetController());
		pc->SetControlRotation(rfs.sightRotation);
		pc->LookUpAtRate(rfs.sightRotation.Pitch);
		AHumanCharacter* hc = static_cast<AHumanCharacter*>(currentHandlePawn_);
		UCameraComponent* cam = hc->GetFollowCamera();
		cam->FieldOfView = rfs.fov;
		cam->AspectRatio = rfs.aspectRation;
	}
}

APawn * APawnHandler::GetCurrentHandlePawn()
{
	return currentHandlePawn_;
}

URoleFollowLabelComponent * APawnHandler::GetRoleLabel()
{
	if (nullptr == currentHandlePawn_)
		return nullptr;
	AHumanCharacter* hc = static_cast<AHumanCharacter*>(currentHandlePawn_);
	if (hc)
		return hc->RoleLabel;
	return nullptr;
}

USpringArmComponent * APawnHandler::GetRoleSpringArm()
{
	if (nullptr == currentHandlePawn_)
		return nullptr;
	AHumanCharacter* hc = static_cast<AHumanCharacter*>(currentHandlePawn_);
	if (hc)
		return hc->GetCameraBoom();
	return nullptr;
}

UCameraComponent * APawnHandler::GetRoleCamera()
{
	if (nullptr == currentHandlePawn_)
		return nullptr;
	AHumanCharacter* hc = static_cast<AHumanCharacter*>(currentHandlePawn_);
	if (hc)
		return hc->GetFollowCamera();
	return nullptr;
}

void APawnHandler::HandlePawn(APawn * pawn)
{
	if (pawn)
	{
		AScenePlayerController* pc = static_cast<AScenePlayerController*>(pawn->GetController());
		if (pc)
		{
			pc->ClearHitResult();
			pc->CleanHitEffect(currentHandlePawn_);
			pc->ResetDefaultCameraTag();
		}
		AHumanCharacter* hc = static_cast<AHumanCharacter*>(pawn);
		hc->VelocityIgnoreCollision = 0.f;
	}
	currentHandlePawn_ = pawn;
	if( nullptr != pawn )
		SetDefaultFlyCamera();
}

void APawnHandler::AttachMeshToHandlePawn(AStaticMeshActor* sma)
{
	AttachMeshToSpecificPawn(currentHandlePawn_,sma);
}

void APawnHandler::AttachMeshToSpecificPawn(APawn * pawn, AStaticMeshActor * sma)
{
	if (nullptr == pawn || nullptr == sma)
		return;
	AHumanCharacter* hc = static_cast<AHumanCharacter*>(pawn);
	if (hc && hc->MeshStaticObject)
	{
		hc->SetHoldOnMesh(sma);
		USceneComponent* sc = hc->GetRootComponent();
		if (SetMeshComponentVisible(true, sc, TEXT("MeshStaticObject")))
		{
			hc->MeshStaticObject->SetStaticMesh(sma->GetStaticMeshComponent()->GetStaticMesh());
			sma->SetActorHiddenInGame(true);
		}
		else
		{
			hc->SetHoldOnMesh(nullptr);
		}
	}
}

void APawnHandler::DetachMeshFromHandlePawn()
{
	DetachMeshFromSpecificPawn(currentHandlePawn_);
};

void APawnHandler::DetachMeshFromSpecificPawn(APawn * pawn)
{
	if (nullptr == pawn)
		return;
	USceneComponent* sc = pawn->GetRootComponent();
	if (SetMeshComponentVisible(false, sc, TEXT("MeshStaticObject")))
	{
		AHumanCharacter* hc = static_cast<AHumanCharacter*>(pawn);
		if (hc && hc->MeshStaticObject)
			hc->MeshStaticObject->SetStaticMesh(nullptr);
		if (hc && hc->GetHoldOnMesh())
		{
			hc->GetHoldOnMesh()->SetActorHiddenInGame(false);
			FVector origin;
			FVector boxExtend;
			FVector up;
			FVector forward;
			hc->GetActorBounds(true, origin, boxExtend);
			up = hc->GetActorUpVector();
			forward = hc->GetActorForwardVector();
			origin = hc->GetActorLocation();
			//
			FVector offsetStartPos = origin + forward*boxExtend.X;
			FVector offsetEndPos = offsetStartPos - up * boxExtend.Z*.618;
			FHitResult hitResult;
			FCollisionQueryParams collisionQueryParam;
			collisionQueryParam.AddIgnoredActor(hc);
			GetWorld()->LineTraceSingleByChannel(hitResult, offsetStartPos, offsetEndPos, ECollisionChannel::ECC_WorldStatic, collisionQueryParam);
			float placeHeight = -hc->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
			placeHeight = FMath::Min(-hitResult.Distance, placeHeight);
			hc->GetHoldOnMesh()->SetActorLocation(offsetStartPos + up * placeHeight);
			hc->SetHoldOnMesh(nullptr);
		}
	}
}

void APawnHandler::SetBindMeshRelativeLocation(FVector location)
{
	if (nullptr == currentHandlePawn_)
		return;
	AHumanCharacter* hc = static_cast<AHumanCharacter*>(currentHandlePawn_);
	if (hc->MeshStaticObject)
		hc->MeshStaticObject->SetRelativeLocation(location);
}

void APawnHandler::SetBindMeshRelativeRotation(FRotator rotation)
{
	if (nullptr == currentHandlePawn_)
		return;
	AHumanCharacter* hc = static_cast<AHumanCharacter*>(currentHandlePawn_);
	if (hc->MeshStaticObject)
		hc->MeshStaticObject->SetRelativeRotation(rotation);
}

void APawnHandler::SetBindMeshRelativeScale(FVector scale)
{
	if (nullptr == currentHandlePawn_)
		return;
	AHumanCharacter* hc = static_cast<AHumanCharacter*>(currentHandlePawn_);
	if (hc->MeshStaticObject)
		hc->MeshStaticObject->SetRelativeScale3D(scale);
}

bool APawnHandler::GetHitResult(FHitResult & hr)
{
	if (nullptr == currentHandlePawn_)
		return false;
	hr.Reset();
	AHumanCharacter* hc = static_cast<AHumanCharacter*>(currentHandlePawn_);
	AScenePlayerController* pc = static_cast<AScenePlayerController*>(hc->GetController());
	if (nullptr == pc || nullptr == pc->GetHitResult().GetActor())
		return false;
	hr = FHitResult(pc->GetHitResult());
	return true;
}

void APawnHandler::Reset()
{
	if (nullptr == currentHandlePawn_)
		return;
	AHumanCharacter* hc = static_cast<AHumanCharacter*>(currentHandlePawn_);
	AScenePlayerController* pc = static_cast<AScenePlayerController*>(hc->GetController());
	currentHandlePawn_ = nullptr;
	if (nullptr == pc || nullptr == pc->GetHitResult().GetActor())
		return;
	pc->ClearHitResult();
	
}

bool APawnHandler::ActiveSprite(int spriteIndex, FString texPath)
{
	AHumanCharacter* hc = static_cast<AHumanCharacter*>(currentHandlePawn_);
	ATextureFinder* texFinder = FindObject<ATextureFinder>(ANY_PACKAGE, TEXT("TextureFinder"));
	if (!(hc && texFinder))
		return false;
	UTexture2D* tex = texFinder->LoadTexture(texPath);
	if (nullptr == tex)
		return false;
	switch (spriteIndex)
	{
	case 1:
		if (hc->Sprite1)
		{
			hc->SpriteCollision1->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			hc->Sprite1->SetHiddenInGame(false);
			hc->Sprite1->SetSprite(tex);
		}
		break;
	case 2:
		if (hc->Sprite2)
		{
			hc->SpriteCollision2->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			hc->Sprite2->SetHiddenInGame(false);
			hc->Sprite2->SetSprite(tex);
		}
		break;
	case 3:
		if (hc->Sprite3)
		{
			hc->SpriteCollision3->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			hc->Sprite3->SetHiddenInGame(false);
			hc->Sprite3->SetSprite(tex);
		}
		break;
	default:
		break;
	}
	return true;
}

bool APawnHandler::IsSpriteActive(int spriteIndex)
{
	AHumanCharacter* hc = static_cast<AHumanCharacter*>(currentHandlePawn_);
	if (nullptr == hc)
		return false;
	switch (spriteIndex)
	{
	case 1:
		if (hc->Sprite1)
			return 1 != hc->Sprite1->bHiddenInGame;
		break;
	case 2:
		if (hc->Sprite2)
			return 1 != hc->Sprite2->bHiddenInGame;
		break;
	case 3:
		if (hc->Sprite3)
			return 1 != hc->Sprite3->bHiddenInGame;
		break;
	default:
		break;
	}
	return false;
}

bool APawnHandler::InactiveSprite(int spriteIndex)
{
	AHumanCharacter* hc = static_cast<AHumanCharacter*>(currentHandlePawn_);
	if (nullptr == hc)
		return false;
	switch (spriteIndex)
	{
	case 1:
		if (hc->Sprite1)
		{
			hc->SpriteCollision1->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			hc->Sprite1->SetHiddenInGame(true);
		}
		break;
	case 2:
		if (hc->Sprite2)
		{
			hc->SpriteCollision2->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			hc->Sprite2->SetHiddenInGame(true);
		}
		break;
	case 3:
		if (hc->Sprite3)
		{
			hc->SpriteCollision3->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			hc->Sprite3->SetHiddenInGame(true);
		}
		break;
	default:
		break;
	}
	return true;
}

bool APawnHandler::ActiveSpriteAgain(int spriteIndex)
{
	AHumanCharacter* hc = static_cast<AHumanCharacter*>(currentHandlePawn_);
	if (nullptr == hc)
		return false;
	switch (spriteIndex)
	{
	case 1:
		if (hc->Sprite1)
		{
			hc->SpriteCollision1->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			hc->Sprite1->SetHiddenInGame(false);
		}
		break;
	case 2:
		if (hc->Sprite2)
		{
			hc->SpriteCollision2->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			hc->Sprite2->SetHiddenInGame(false);
		}
		break;
	case 3:
		if (hc->Sprite3)
		{
			hc->SpriteCollision3->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			hc->Sprite3->SetHiddenInGame(false);
		}
		break;
	default:
		break;
	}
	return true;
}

bool APawnHandler::HitOnSpriteTest(int spriteIndex)
{
	AHumanCharacter* hc = static_cast<AHumanCharacter*>(currentHandlePawn_);
	if (nullptr == hc)
		return false;
	AScenePlayerController* pc = static_cast<AScenePlayerController*>(hc->GetController());
	if (nullptr == pc )
		return false;
	const FHitResult& hr = pc->GetHitResult();
	switch (spriteIndex)
	{
	case 1:
		return hr.GetComponent() == hc->SpriteCollision1;
	case 2:
		return hr.GetComponent() == hc->SpriteCollision2;
	case 3:
		return hr.GetComponent() == hc->SpriteCollision3;
	default:
		break;
	}
	return false;
}

bool APawnHandler::HitOnSpriteTestAny()
{
	AHumanCharacter* hc = static_cast<AHumanCharacter*>(currentHandlePawn_);
	if (nullptr == hc)
		return false;
	AScenePlayerController* pc = static_cast<AScenePlayerController*>(hc->GetController());
	if (nullptr == pc)
		return false;
	const FHitResult& hr = pc->GetHitResult();
	return hr.GetComponent() == hc->SpriteCollision1
		|| hr.GetComponent() == hc->SpriteCollision2
		|| hr.GetComponent() == hc->SpriteCollision3;
}

FString APawnHandler::GetSpriteTexPath(int spriteIndex)
{
	FString texPath;
	AHumanCharacter* hc = static_cast<AHumanCharacter*>(currentHandlePawn_);
	if (nullptr == hc)
		return texPath;
	switch (spriteIndex)
	{
	case 1:
		texPath = hc->Sprite1->Sprite->GetPathName();
		break;
	case 2:
		texPath = hc->Sprite2->Sprite->GetPathName();
		break;
	case 3:
		texPath = hc->Sprite3->Sprite->GetPathName();
		break;
	default:
		break;
	}
	return texPath;
}

bool APawnHandler::ParticlePress()
{
	AHumanCharacter* hc = static_cast<AHumanCharacter*>(currentHandlePawn_);
	if (nullptr == hc)
		return false;
	hc->ParticlePress();
	AHumanCharacter::FSynchronizeSender::SyncToServer(hc->GetName());
	return true;
}

bool APawnHandler::ParticleRelease()
{
	AHumanCharacter* hc = static_cast<AHumanCharacter*>(currentHandlePawn_);
	if (nullptr == hc)
		return false;
	hc->ParticleRelease();
	AHumanCharacter::FSynchronizeSender::SyncToServer(hc->GetName());
	return true;
}

bool APawnHandler::ParticleIsActive()
{
	AHumanCharacter* hc = static_cast<AHumanCharacter*>(currentHandlePawn_);
	if (nullptr == hc)
		return false;
	return hc->ParticleIsActive();
}

bool APawnHandler::ParticleSpecificPawnPress(APawn * pawn)
{
	AHumanCharacter* hc = static_cast<AHumanCharacter*>(pawn);
	if (nullptr == hc)
		return false;
	hc->ParticlePress();
	//don't need to send.
	return true;
}

bool APawnHandler::ParticleSpecificPawnRelease(APawn * pawn)
{
	AHumanCharacter* hc = static_cast<AHumanCharacter*>(pawn);
	if (nullptr == hc)
		return false;
	hc->ParticleRelease();
	return true;
}

bool APawnHandler::ParticleSpecificPawnIsActive(APawn * pawn)
{
	AHumanCharacter* hc = static_cast<AHumanCharacter*>(pawn);
	if (nullptr == hc)
		return false;
	return hc->ParticleIsActive();
}

bool APawnHandler::JumpPress()
{
	AHumanCharacter* hc = static_cast<AHumanCharacter*>(currentHandlePawn_);
	if (nullptr == hc)
		return false;
	hc->JumpPress();
	return true;
}

bool APawnHandler::JumpRelease()
{
	AHumanCharacter* hc = static_cast<AHumanCharacter*>(currentHandlePawn_);
	if (nullptr == hc)
		return false;
	hc->JumpRelease();
	return true;
}

bool APawnHandler::JumpIsPress()
{
	AHumanCharacter* hc = static_cast<AHumanCharacter*>(currentHandlePawn_);
	if (nullptr == hc)
		return false;
	return hc->bJumpButtonDown;
}

FVector APawnHandler::GetPawnLocation()
{
	AHumanCharacter* hc = static_cast<AHumanCharacter*>(currentHandlePawn_);
	if (nullptr == hc)
		return FVector();
	FVector origin;
	FVector boxExtend;
	hc->GetActorBounds(true, origin, boxExtend);
	return origin;
}

bool APawnHandler::SetLogicKey(const FString & key, bool val)
{
	AHumanCharacter* hc = static_cast<AHumanCharacter*>(currentHandlePawn_);
	if (nullptr == hc)
		return false;
	TMap<FString, bool>& lk = hc->GetLogicKeys();
	lk.FindOrAdd(key) = val;
	AHumanCharacter::FSynchronizeSender::SyncToServer(hc->GetName());
	return true;
}

bool APawnHandler::RemoveLogicKey(const FString & key)
{
	AHumanCharacter* hc = static_cast<AHumanCharacter*>(currentHandlePawn_);
	if (nullptr == hc)
		return false;
	return 0 <= hc->GetLogicKeys().Remove(key);
}

bool APawnHandler::QueryLogicKey(const FString & key)
{
	AHumanCharacter* hc = static_cast<AHumanCharacter*>(currentHandlePawn_);
	if (nullptr == hc)
		return false;
	return hc->QueryLogicKey(key);
}

bool APawnHandler::EnableTouchLevel(ETouchEnableLevel tel)
{
	if (AScenePlayerController::Handler)
		AScenePlayerController::Handler->EnableTouchLevel(tel);
	return false;
}

bool APawnHandler::SetDefaultFlyCamera()
{
	AHumanCharacter* hc = static_cast<AHumanCharacter*>(currentHandlePawn_);
	if (nullptr == hc)
		return false;
	AScenePlayerController* pc = static_cast<AScenePlayerController*>(hc->GetController());
	if (nullptr == pc)
		return false;
	pc->SetDefaultCameraParam(hc->DefaultCameraRoator,hc->DefaultArmLength*1.2f);
	return true;
}

bool APawnHandler::ApplyDefaultFlyCamera()
{
	AHumanCharacter* hc = static_cast<AHumanCharacter*>(currentHandlePawn_);
	if (nullptr == hc)
		return false;
	AScenePlayerController* pc = static_cast<AScenePlayerController*>(hc->GetController());
	if (nullptr == pc)
		return false;
	pc->ApplyDefaultCameraParam();
	return true;
}

bool APawnHandler::SetAsDefaultFlyCameraRotation(const FRotator & r)
{
	AHumanCharacter* hc = static_cast<AHumanCharacter*>(currentHandlePawn_);
	if (nullptr == hc)
		return false;
	AScenePlayerController* pc = static_cast<AScenePlayerController*>(hc->GetController());
	if (nullptr == pc)
		return false;
	pc->SetDefaultCameraParam(r, hc->DefaultArmLength*1.2f);
	return true;
}

bool APawnHandler::EnableAcceptControllerEvent(bool bEnable)
{
	//GEngine->AddOnScreenDebugMessage(
	//	-1,
	//	20.f,
	//	FColor::Red,
	//	FString::Printf(TEXT("enable accept controller: ~> %s"),
	//		bEnable?TEXT("true"):TEXT("false")));
	if (AScenePlayerController::Handler)
		AScenePlayerController::Handler->EnableTick(bEnable);
	return true;
}

void APawnHandler::ChangeScalarParameterOfCollection(const FString & collectionPath, FName paramName, float newVal)
{
	UMaterialParameterCollection* pc = LoadObject<UMaterialParameterCollection>(NULL,
		*collectionPath,
		NULL,
		LOAD_None,
		NULL);
	UMaterialParameterCollectionInstance* pci = GetWorld()->GetParameterCollectionInstance(pc);
	if (pci)
	{
		pci->SetScalarParameterValue(paramName,newVal);
	}
}

void APawnHandler::ChangeVectorParameterOfCollection(const FString & collectionPath, FName paramName, const FLinearColor& newVal)
{
	UMaterialParameterCollection* pc = LoadObject<UMaterialParameterCollection>(NULL,
		*collectionPath,
		NULL,
		LOAD_None,
		NULL);
	UMaterialParameterCollectionInstance* pci = GetWorld()->GetParameterCollectionInstance(pc);
	if (pci)
	{
		pci->SetVectorParameterValue(paramName, newVal);
	}
}

bool APawnHandler::MeshIsTakeOn()
{
	AHumanCharacter* hc = static_cast<AHumanCharacter*>(currentHandlePawn_);
	if (nullptr == hc)
		return false;
	return nullptr != hc->MeshStaticObject
		&& nullptr != hc->MeshStaticObject->GetStaticMesh();
}

bool APawnHandler::BindBoneToSocket(const FName & socketName)
{
	return BindSpecificPawnBoneToSocket(currentHandlePawn_, socketName);
}

bool APawnHandler::BindSpecificPawnBoneToSocket(const APawn * pawn, const FName & socketName)
{
	const AHumanCharacter* hc = static_cast<const AHumanCharacter*>(pawn);
	if (nullptr == hc)
		return false;
	if (hc->MeshSlot)
	{
		hc->MeshSlot->DetachFromComponent(FDetachmentTransformRules(EDetachmentRule::KeepWorld, true));
		hc->MeshSlot->AttachToComponent(hc->GetMesh(),
			FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true),
			socketName);
		return true;
	}
	return false;
}


