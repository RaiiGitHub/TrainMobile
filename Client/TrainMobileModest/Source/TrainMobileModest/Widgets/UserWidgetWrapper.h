// As Part of GuangZhou Training.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UserWidgetWrapper.generated.h"

/**
 * 
 */
UCLASS()
class TRAINMOBILEMODEST_API UUserWidgetWrapper 
	: public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	void SetupWidgetLogic(const FString LogicName);

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	void OnConstruct();

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	void OnDestruct();

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	void OnButtonClick(const FString ObjectName);

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	void OnTextChanged(const FString ObjectName,const FText Text);

protected:
	TSharedPtr<class WidgetLogicBase> WidgetLogicHandler;
};
