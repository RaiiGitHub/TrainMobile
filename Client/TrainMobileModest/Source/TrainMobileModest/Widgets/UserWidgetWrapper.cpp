// As Part of GuangZhou Training.

#include "UserWidgetWrapper.h"
#include "TrainMobileModest.h"
#include "WidgetLogicBase.h"
#include "Runtime/UMG/Public/UMG.h"
#include "Runtime/UMG/Public/UMGStyle.h"
#include "Runtime/UMG/Public/Slate/SObjectWidget.h"
#include "Runtime/UMG/Public/IUMGModule.h"
#include "Runtime/UMG/Public/Blueprint/UserWidget.h"

void UUserWidgetWrapper::SetupWidgetLogic(const FString LogicName)
{
	WidgetLogicHandler = WidgetLogicFactoryManager::GetInstance()->GetFactory(LogicName)->CreateWidgetLogic();
	WidgetLogicHandler->SetUserWidget(this);
}

void UUserWidgetWrapper::OnConstruct()
{
	if (WidgetLogicHandler.Get())
		WidgetLogicHandler->OnConstruct();
}

void UUserWidgetWrapper::OnDestruct()
{
	if (WidgetLogicHandler.Get())
		WidgetLogicHandler->OnDestruct();
}

void UUserWidgetWrapper::OnButtonClick(const FString ObjectName)
{
	if (WidgetLogicHandler.Get())
		WidgetLogicHandler->OnButtonClick(ObjectName);
}

void UUserWidgetWrapper::OnTextChanged(const FString ObjectName, const FText Text)
{
	if (WidgetLogicHandler.Get())
		WidgetLogicHandler->OnTextChanged(ObjectName, Text);
}

WidgetLogicBase * UUserWidgetWrapper::GetWidgetLogic()
{
	return WidgetLogicHandler.Get();
}
