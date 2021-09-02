// As Part of GuangZhou Training.

#include "WidgetLogicBase.h"
#include "TrainMobileModest.h"

void WidgetLogicBase::SetName(const FString & name)
{
	Name = name;
}

FString WidgetLogicBase::GetName()
{
	return Name;
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
