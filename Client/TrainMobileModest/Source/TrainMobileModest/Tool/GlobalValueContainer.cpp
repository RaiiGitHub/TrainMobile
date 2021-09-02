// As Part of GuangZhou Training.

#include "GlobalValueContainer.h"
#include "TrainMobileModest.h"
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
TMap<FString, FString>             UGlobalValueContainer::MapPswContent;
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

bool UGlobalValueContainer::AddPswContent(const FString & key, const FString & psw)
{
	bool add_ok = false;
	FString* str = MapPswContent.Find(key);
	if (nullptr == str)
	{
		MapPswContent.Add(key, psw);
		add_ok = true;
	}
	else
	{
		add_ok = false;
		const int len = psw.Len();
		int pre_len = str->Len();
		for (int i = 0; i < pre_len - len; ++i)
			str->RemoveAt(str->Len() - 1);
		pre_len = str->Len();
		for (int i = 0; i < len; ++i)
		{
			const FString c = psw.Mid(i, 1);
			if (!c.Equals("*"))
			{
				add_ok = true;
				if (i < pre_len)
				{
					str->RemoveAt(i);
					str->InsertAt(i, c);
				}
				else
					str->Append(c);
			}
		}
	}
	return add_ok;
}

int UGlobalValueContainer::RemovePswContent(const FString & key)
{
	return MapPswContent.Remove(key);
}

void UGlobalValueContainer::ClearPswContent(const FString & key)
{
	MapPswContent.Empty();
}

FString UGlobalValueContainer::GetPswContent(const FString& key, bool encrypt)
{
	FString content;
	const FString* str = MapPswContent.Find(key);
	if (nullptr == str)
		return content;
	if (encrypt)
	{
		int len = str->Len();
		for (int i = 0; i < len; ++i)
			content.Append("*");
	}
	else
		content = *str;
	return content;
}

void UGlobalValueContainer::AutoWrapText(FString& out_format, int32& out_height, int32& out_width, const FString & origin, const float line_maxwidth, const FSlateFontInfo& fi)
{
	if (!fi.HasValidFont()
		|| origin.IsEmpty()
		|| 0 >= line_maxwidth)
		return;
	FString origin_no_r = origin.Replace(TEXT("\r\n"), TEXT("\n"));
	const UFont* font = static_cast<const UFont*>(fi.FontObject);
	out_format.Empty();
	out_height = 0.f;
	out_width = 0.f;
	int content_len = origin_no_r.Len();
	int origin_width = font->GetStringSize(*origin_no_r);
	float cur_width = 0.f;
	const TCHAR sep = '\n';
	const float rate = fi.Size / 10.f;
	for (int i = 0; i < content_len; ++i)
	{
		if (i > 0 && out_format.Right(1)[0] == sep && origin_no_r[i] == sep)
			continue;
		float char_width = 0.f;
		float char_hegiht = 0.f;
		font->GetCharSize(origin_no_r[i], char_width, char_hegiht);
		cur_width += char_width * rate;
		out_format.AppendChar(origin_no_r[i]);
		if (0 == out_height)
			out_height = char_hegiht * rate;
		out_width = FMath::Max((float)out_width, cur_width);
		if (origin_no_r[i] == sep)
		{
			out_height += char_hegiht * rate;
			cur_width = 0.f;
		}
		else if (cur_width >= line_maxwidth && i != content_len - 1)
		{
			out_format.AppendChar(sep);
			out_height += char_hegiht * rate;
			cur_width = 0.f;

		}
	}
}
