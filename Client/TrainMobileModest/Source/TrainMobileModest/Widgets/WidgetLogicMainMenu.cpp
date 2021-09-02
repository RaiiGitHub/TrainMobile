#include "WidgetLogicMainMenu.h"
#include "TrainMobileModest.h"
#include "WidgetLogicReturn.h"
#include "WidgetLogicGameMenu.h"
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

WIDGET_LOGIC_FACTORY_REG(WidgetLogicMainMenu);
IMPL_GETBLUEPRINTCLASSPATH(WidgetLogicMainMenu, WidgetBlueprint'/Game/Widget/MainMenu.MainMenu_C');

WidgetLogicMainMenu::WidgetLogicMainMenu()
{
}

WidgetLogicMainMenu::~WidgetLogicMainMenu()
{
}

void WidgetLogicMainMenu::UpdateUserLabel()
{
	UTextBlock* pText = Cast<UTextBlock>(UserWidget->GetWidgetFromName(TEXT("info")));
	if (pText)
	{
		TSharedPtr<SUser> u = GetUserProducer()->GetClientUser();
		if (u.Get())
		{
			FString content = FString::Printf(TEXT("%s%s%s%s%s/%s")
				, *u->name_
				, *UGlobalValueContainer::GetSystemValue(TEXT("mainmenu.lastlogin.1"))
				, *u->last_login_time_
				, *UGlobalValueContainer::GetSystemValue(TEXT("mainmenu.lastlogin.2"))
				, *u->version_
			    , *FUserProducer::InnerVersion);
			pText->SetText(FText::FromString(content));
		}
	}
}

void WidgetLogicMainMenu::EnterExerciseSystem()
{
	{
		//create the game menu as the homepage for exercise-system.
		TSubclassOf<UUserWidget> WidgetClass = WidgetLogicBase::CreateUserWidgetClass(UserWidget, WidgetLogicGameMenu::BlueprintClassPath);
		if (UUserWidget* pUserWidget = UGlobalValueContainer::CreateUserWidget(WidgetClass))
		{
			if (WidgetLogicGameMenu* pLogic = static_cast<WidgetLogicGameMenu*>(Cast<UUserWidgetWrapper>(pUserWidget)->GetWidgetLogic()))
			{
				//to be written here.
			}
		}
	}


	{
		//create a return widget.
		TSubclassOf<UUserWidget> WidgetClass = WidgetLogicBase::CreateUserWidgetClass(UserWidget, WidgetLogicReturn::BlueprintClassPath);
		if (UUserWidget* pUserWidget = UGlobalValueContainer::CreateUserWidget(WidgetClass))
		{
			if (WidgetLogicReturn* pLogic = static_cast<WidgetLogicReturn*>(Cast<UUserWidgetWrapper>(pUserWidget)->GetWidgetLogic()))
			{
				//to be written here.
			}
		}
	}

	//as well as remove itself.
	UGlobalValueContainer::RemoveWidgetFromViewport(UserWidget);
}

void WidgetLogicMainMenu::EnterVideoLearningSystem()
{
	{
		//create the video-learning homepage.
		TSubclassOf<UUserWidget> WidgetClass = WidgetLogicBase::CreateUserWidgetClass(UserWidget, WidgetLogicVideoLearning::BlueprintClassPath);
		if (UUserWidget* pUserWidget = UGlobalValueContainer::CreateUserWidget(WidgetClass))
		{
			if (WidgetLogicVideoLearning* pLogic = static_cast<WidgetLogicVideoLearning*>(Cast<UUserWidgetWrapper>(pUserWidget)->GetWidgetLogic()))
			{
				//to be written here.
			}
		}
	}

	{
		//create a return widget.
		TSubclassOf<UUserWidget> WidgetClass = WidgetLogicBase::CreateUserWidgetClass(UserWidget, WidgetLogicReturn::BlueprintClassPath);
		if (UUserWidget* pUserWidget = UGlobalValueContainer::CreateUserWidget(WidgetClass))
		{
			if (WidgetLogicReturn* pLogic = static_cast<WidgetLogicReturn*>(Cast<UUserWidgetWrapper>(pUserWidget)->GetWidgetLogic()))
			{
				//to be written here.
			}
		}
	}

	//as well as remove itself.
	UGlobalValueContainer::RemoveWidgetFromViewport(UserWidget);
}

void WidgetLogicMainMenu::OnConstruct()
{
	UpdateUserLabel();
}

void WidgetLogicMainMenu::OnDestruct()
{
}

void WidgetLogicMainMenu::OnButtonClick(const FString ObjectName)
{
	if (nullptr == UTrainGameInstance::Instance || nullptr == UserWidget)
		return;
	if (ObjectName.Equals(TEXT("EnterVideoLearningSystem"), ESearchCase::IgnoreCase))
	{
		EnterVideoLearningSystem();
	}
	else if (ObjectName.Equals(TEXT("EnterExerciseSystem"), ESearchCase::IgnoreCase))
	{
		EnterExerciseSystem();
	}
}


