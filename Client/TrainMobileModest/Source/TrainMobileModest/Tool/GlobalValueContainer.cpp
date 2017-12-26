// As Part of GuangZhou Training.

#include "TrainMobileModest.h"
#include "GlobalValueContainer.h"
#include "Engine/DataTable.h"
#include "TaskData.h"
#include "Widgets/WidgetLogicBase.h"
#include "Widgets/UserWidgetWrapper.h"
#include "Runtime/UMG/Public/UMG.h"
#include "Runtime/UMG/Public/UMGStyle.h"
#include "Runtime/UMG/Public/Slate/SObjectWidget.h"
#include "Runtime/UMG/Public/IUMGModule.h"
#include "Runtime/UMG/Public/Blueprint/UserWidget.h"

TMap<FString,AActor*>             UGlobalValueContainer::GlobeActors;
TMap<FString,FString>             UGlobalValueContainer::SystemKeyValues;
TArray<AActor*>                   UGlobalValueContainer::MiniMapActors;
FDateTime                         UGlobalValueContainer::GameBeginTime;
TArray<UUserWidget*>              UGlobalValueContainer::UserWidgets;

void UGlobalValueContainer::SetGameBeginTime(FDateTime dateTime)
{
	GameBeginTime = dateTime;
}

FDateTime & UGlobalValueContainer::GetGameBeginTime()
{
	// TODO: insert return statement here
	return GameBeginTime;
}

void UGlobalValueContainer::AddGlobeActor(FString Key, AActor * pActor)
{
	RemoveGlobeActor(Key);
	GlobeActors.Add(Key, pActor);
}

void UGlobalValueContainer::RemoveGlobeActor(FString Key)
{
	GlobeActors.Remove(Key);
}

AActor* UGlobalValueContainer::FindGlobeActor(FString Key)
{
	AActor* pVal = GlobeActors.FindRef(Key);
	if (nullptr != pVal)
		return pVal;
	return nullptr;
}

FString UGlobalValueContainer::GetGlobeActorKey(AActor * pActor)
{
	for (auto& a : GlobeActors)
	{
		if (a.Value == pActor)
			return a.Key;
	}
	return FString();
}

void UGlobalValueContainer::AssignSystemKeyValues()
{
	FString skv_path = TEXT("DataTable'/Game/TaskData/SystemKeyValue.SystemKeyValue'");
	UDataTable* skv = LoadObject<UDataTable>(NULL, *skv_path);
	if (nullptr != skv)
	{
		static const FString ContextString(TEXT("AssignSystemKeyValues"));
		TArray<FTipCharacters*> tips_data;
		skv->GetAllRows<FTipCharacters>(ContextString, tips_data);
		for (auto& tc : tips_data)
			SystemKeyValues.FindOrAdd(tc->Key) = tc->Value;
	}
}

FString UGlobalValueContainer::GetSystemValue(const FString & key)
{
	FString* pVal = SystemKeyValues.Find(key);
	if (nullptr != pVal)
		return *pVal;
	return FString();
}

UUserWidget * UGlobalValueContainer::CreateUserWidget(TSubclassOf<UUserWidget> widgetClass, bool bAddToViewport, int zOrder)
{
	if (GEngine && widgetClass.Get())
	{
		TArray<APlayerController*> lpc;
		GEngine->GetAllLocalPlayerControllers(lpc);
		if (lpc.Num() > 0)
		{
			UUserWidget* pWidget = CreateWidget<UUserWidget>(lpc[0],widgetClass);
			if (pWidget )
			{
				if(bAddToViewport)
					pWidget->AddToViewport(zOrder);
				UserWidgets.Add(pWidget);
			}
			return pWidget;
		}
		
	}
	return nullptr;
}

void UGlobalValueContainer::AddWidgetToViewport(UUserWidget * widget, int zOrder)
{
	if (widget)
		widget->AddToViewport(zOrder);
}

void UGlobalValueContainer::RemoveWidgetFromViewport(UUserWidget * widget)
{
	if (widget)
	{
		UserWidgets.Remove(widget);
		widget->RemoveFromViewport();
	}
}

UUserWidget* UGlobalValueContainer::GetWidgetFromViewport(const FString & logicName)
{
	for (auto& w : UserWidgets)
	{
		WidgetLogicBase* pLogic = Cast<UUserWidgetWrapper>(w)->GetWidgetLogic();
		if (pLogic && pLogic->GetName().Equals(logicName, ESearchCase::IgnoreCase))
			return w;//return the first one(if there were multi userwidgets with the same logic-name )
	}
	return nullptr;
}

void UGlobalValueContainer::RemoveAllWidgetsFromViewport()
{
	for (auto& w : UserWidgets)
		w->RemoveFromViewport();
}

