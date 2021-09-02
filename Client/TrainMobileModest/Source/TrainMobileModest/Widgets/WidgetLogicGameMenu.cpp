#include "WidgetLogicGameMenu.h"
#include "TrainMobileModest.h"
#include "TrainGameInstance.h"
#include "Blueprint/UserWidget.h"
#include "GlobalValueContainer.h"
#include "UserWidgetWrapper.h"
#include "DisplayContent.h"
#include "TrainOnline.h"

#include "Runtime/UMG/Public/UMG.h"
#include "Runtime/UMG/Public/UMGStyle.h"
#include "Runtime/UMG/Public/Slate/SObjectWidget.h"
#include "Runtime/UMG/Public/IUMGModule.h"
#include "Runtime/UMG/Public/Blueprint/UserWidget.h"

WIDGET_LOGIC_FACTORY_REG(WidgetLogicGameMenu);
IMPL_GETBLUEPRINTCLASSPATH(WidgetLogicGameMenu, WidgetBlueprint'/Game/Widget/GameMenuWidget.GameMenuWidget_C');

WidgetLogicGameMenu::WidgetLogicGameMenu()
{
}

WidgetLogicGameMenu::~WidgetLogicGameMenu()
{
}

void WidgetLogicGameMenu::OnConstruct()
{
}

void WidgetLogicGameMenu::OnDestruct()
{
}

void WidgetLogicGameMenu::OnButtonClick(const FString ObjectName)
{
	if (nullptr == UTrainGameInstance::Instance || nullptr == UserWidget)
		return;
	if (ObjectName.Equals(TEXT(""), ESearchCase::IgnoreCase))
	{
	}
}


