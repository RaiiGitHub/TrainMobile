#include "TrainMobileModest.h"
#include "WidgetLogicSimpleMessage.h"
#include "TrainGameInstance.h"
#include "Blueprint/UserWidget.h"
#include "GlobalValueContainer.h"
#include "DisplayContent.h"
#include "Runtime/UMG/Public/UMG.h"
#include "Runtime/UMG/Public/UMGStyle.h"
#include "Runtime/UMG/Public/Slate/SObjectWidget.h"
#include "Runtime/UMG/Public/IUMGModule.h"
#include "Runtime/UMG/Public/Blueprint/UserWidget.h"

WIDGET_LOGIC_FACTORY_REG(WidgetLogicSimpleMessage);

WidgetLogicSimpleMessage::WidgetLogicSimpleMessage()
	:Caller(nullptr)
{
}


WidgetLogicSimpleMessage::~WidgetLogicSimpleMessage()
{
}

void WidgetLogicSimpleMessage::OnButtonClick(const FString ObjectName)
{
	if (nullptr == UTrainGameInstance::Instance || nullptr == UserWidget)
		return;
	if (ObjectName.Equals(TEXT("login"), ESearchCase::IgnoreCase))
	{
		
	}
}


