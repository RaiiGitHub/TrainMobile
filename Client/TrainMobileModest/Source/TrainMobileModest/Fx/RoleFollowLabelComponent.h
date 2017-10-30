// As Part of GuangZhou Training.

#pragma once

#include "Components/TextRenderComponent.h"
#include "RoleFollowLabelComponent.generated.h"

UCLASS()
class TRAINMOBILEMODEST_API URoleFollowLabelComponent : public UTextRenderComponent
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
		void UpdateRoleLabel();
	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
		void SetRoleLabelText(const FText& content );
};
