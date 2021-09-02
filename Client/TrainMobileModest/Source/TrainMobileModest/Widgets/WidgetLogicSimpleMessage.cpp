#include "WidgetLogicSimpleMessage.h"
#include "TrainMobileModest.h"
#include "TrainGameInstance.h"
#include "Blueprint/UserWidget.h"
#include "GlobalValueContainer.h"
#include "UserWidgetWrapper.h"
#include "Runtime/UMG/Public/UMG.h"
#include "Runtime/UMG/Public/UMGStyle.h"
#include "Runtime/UMG/Public/Slate/SObjectWidget.h"
#include "Runtime/UMG/Public/IUMGModule.h"
#include "Runtime/UMG/Public/Blueprint/UserWidget.h"

WIDGET_LOGIC_FACTORY_REG(WidgetLogicSimpleMessage);
IMPL_GETBLUEPRINTCLASSPATH(WidgetLogicSimpleMessage, WidgetBlueprint'/Game/Widget/MessageBoxSimpleWidget.MessageBoxSimpleWidget_C');

WidgetLogicSimpleMessage::WidgetLogicSimpleMessage()
	:Caller(nullptr)
{
}


WidgetLogicSimpleMessage::~WidgetLogicSimpleMessage()
{
}

void WidgetLogicSimpleMessage::SetWidgetCaller(UUserWidget * pWidget)
{
	Caller = pWidget;
}

void WidgetLogicSimpleMessage::SetTitle(const FString & title)
{
	UTextBlock* pText = Cast<UTextBlock>(UserWidget->GetWidgetFromName(TEXT("title")));
	if (pText)
	{
		pText->SetText(FText::FromString(title));
	}
}

void WidgetLogicSimpleMessage::SetContent(const FString & content)
{
	UTextBlock* pText = Cast<UTextBlock>(UserWidget->GetWidgetFromName(TEXT("content")));
	if (pText)
	{
		pText->SetText(FText::FromString(content));
	}
}

void WidgetLogicSimpleMessage::SetStyle(bool bContainOkButton, bool bContainCancelButton)
{
	if (!bContainOkButton)
	{
		UButton* pButton = Cast<UButton>(UserWidget->GetWidgetFromName(TEXT("ok")));
		pButton->RemoveFromParent();
	}
	if (!bContainCancelButton)
	{
		UButton* pButton = Cast<UButton>(UserWidget->GetWidgetFromName(TEXT("cancel")));
		pButton->RemoveFromParent();
	}
}

void WidgetLogicSimpleMessage::OnButtonClick(const FString ObjectName)
{
	if (nullptr == UTrainGameInstance::Instance || nullptr == UserWidget)
		return;
	if (ObjectName.Equals(TEXT("ok"), ESearchCase::IgnoreCase))
	{
		if (Caller)
			Cast<UUserWidgetWrapper>(Caller)->OnButtonClick(ObjectName);
		UGlobalValueContainer::RemoveWidgetFromViewport(UserWidget);//removed msg widget.
	}
	else if (ObjectName.Equals(TEXT("cancel"), ESearchCase::IgnoreCase))
	{
		UGlobalValueContainer::RemoveWidgetFromViewport(UserWidget);//removed msg widget.
	}
}

void WidgetLogicSimpleMessage::OnConstruct()
{

}

void WidgetLogicSimpleMessage::OnDestruct()
{
}


