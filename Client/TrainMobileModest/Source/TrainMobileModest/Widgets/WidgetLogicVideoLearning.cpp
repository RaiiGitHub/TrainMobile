#include "TrainMobileModest.h"
#include "WidgetLogicVideoLearning.h"
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

WIDGET_LOGIC_FACTORY_REG(WidgetLogicVideoLearning);
IMPL_GETBLUEPRINTCLASSPATH(WidgetLogicVideoLearning, WidgetBlueprint'/Game/Widget/VideoLearningWidget.VideoLearningWidget_C');

WidgetLogicVideoLearning::WidgetLogicVideoLearning()
{
}

WidgetLogicVideoLearning::~WidgetLogicVideoLearning()
{
}

void WidgetLogicVideoLearning::OnConstruct()
{
}

void WidgetLogicVideoLearning::OnDestruct()
{
}

void WidgetLogicVideoLearning::OnButtonClick(const FString ObjectName)
{
	if (nullptr == UTrainGameInstance::Instance || nullptr == UserWidget)
		return;
	if (ObjectName.Equals(TEXT(""), ESearchCase::IgnoreCase))
	{
	}
}


