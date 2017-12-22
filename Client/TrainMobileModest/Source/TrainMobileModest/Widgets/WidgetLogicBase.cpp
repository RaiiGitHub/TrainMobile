// As Part of GuangZhou Training.

#include "TrainMobileModest.h"
#include "WidgetLogicBase.h"

void WidgetLogicBase::SetName(const FString & name)
{
	WidgetName = name;
}

FString WidgetLogicBase::GetName()
{
	return WidgetName;
}

void WidgetLogicBase::SetUserWidget(UUserWidget * widget)
{
	UserWidget = widget;
}

UUserWidget * WidgetLogicBase::GetUserWidget()
{
	return UserWidget;
}

TSubclassOf<UUserWidget> WidgetLogicBase::CreateUserWidgetClass(UObject* context, const FString& path)
{
	if (context && !path.IsEmpty())
		return LoadClass<UUserWidget>(context, *path);
	return TSubclassOf<UUserWidget>();
}

void WidgetLogicFactoryManager::AddFactory(WidgetLogicFactoryPtr & factory)
{
	WidgetFactories.AddUnique(factory);
}

WidgetLogicFactoryBase* WidgetLogicFactoryManager::GetFactory(const FString & name)
{
	for (auto& f : WidgetFactories)
	{
		if (f.Get()->GetName().Equals(name, ESearchCase::IgnoreCase))
			return f.Get();
	}
	return nullptr;
}
