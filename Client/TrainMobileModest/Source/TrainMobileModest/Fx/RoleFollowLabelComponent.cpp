// As Part of GuangZhou Training.

#include "TrainMobileModest.h"
#include "RoleFollowLabelComponent.h"
#include "Kismet/GameplayStatics.h"

void URoleFollowLabelComponent::UpdateRoleLabel()
{
	USceneComponent* sc = UGameplayStatics::GetPlayerCameraManager(this,0)->GetTransformComponent();
	if (nullptr != sc)
	{
		float fFaceYaw = sc->GetComponentRotation().Yaw + 180.f;
		FRotator rotate = GetComponentRotation();
		rotate.Yaw = fFaceYaw;
		SetWorldRotation(rotate);
	}
}

void URoleFollowLabelComponent::SetRoleLabelText(const FText& content)
{
	SetText(content);
}

