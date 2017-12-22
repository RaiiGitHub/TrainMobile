// As Part of GuangZhou Training.

#pragma once

#include "Singleton.h"

class WidgetLogicBase
{
public:
	WidgetLogicBase():UserWidget(nullptr){}
	virtual ~WidgetLogicBase() {}
public:
	void               SetName(const FString& name);
	FString            GetName();
	virtual FString    GetBlueprintClassPath() { return FString(); }
	void               SetUserWidget(UUserWidget* widget);
	UUserWidget*       GetUserWidget();
public:
	virtual void       OnButtonClick(const FString ObjectName) {}
	virtual void       OnTextChanged(const FString ObjectName,const FText Text) {}
	virtual void       OnConstruct() {}
	virtual void       OnDestruct() {}
public:
	static TSubclassOf<UUserWidget> CreateUserWidgetClass(UObject* context,const FString& path);

protected:
	class UUserWidget* UserWidget;
	FString WidgetName;
};
typedef TSharedPtr<WidgetLogicBase> WidgetLogicPtr;
typedef TArray<WidgetLogicPtr> WidgetLogicArray;
#define DECLARE_GETBLUEPRINTCLASSPATH(T) static FString BlueprintClassPath; virtual FString GetBlueprintClassPath() override { return T::BlueprintClassPath;}
#define IMPL_GETBLUEPRINTCLASSPATH(T,P) FString T::BlueprintClassPath = TEXT(#P);

class WidgetLogicFactoryBase 
{
public:
	WidgetLogicFactoryBase() {}
	virtual ~WidgetLogicFactoryBase() {}
public:
	virtual TSharedPtr<WidgetLogicBase> CreateWidgetLogic() = 0;
	inline void    SetName(const FString& name) { LogicName = name; }
	inline FString GetName() { return LogicName; }
protected:
	FString LogicName;
};
typedef TSharedPtr<WidgetLogicFactoryBase> WidgetLogicFactoryPtr;
typedef TArray<WidgetLogicFactoryPtr> WidgetLogicFactoryArray;

class WidgetLogicFactoryManager
	:public MobileStudio::Singleton<WidgetLogicFactoryManager>
{
public:
	WidgetLogicFactoryManager() {}
	~WidgetLogicFactoryManager() {}
public:
	void AddFactory(WidgetLogicFactoryPtr& factory);
	WidgetLogicFactoryBase* GetFactory(const FString& name);
protected:
	WidgetLogicFactoryArray WidgetFactories;
};

template <typename T>
class WidgetLogicFactory
	:public WidgetLogicFactoryBase
{
public:
	WidgetLogicFactory() {}
	~WidgetLogicFactory() {}
public:
	TSharedPtr<WidgetLogicBase> CreateWidgetLogic()
	{
		WidgetLogicBase* pLogic = new T;
		pLogic->SetName(LogicName);
		return MakeShareable(pLogic);
	}

};

template <typename T>
class WidgetLogicFactoryAutoRegister
{
public:
	WidgetLogicFactoryAutoRegister(const FString& name)
	{
		WidgetLogicFactoryBase* pFactory = new WidgetLogicFactory<T>;
		pFactory->SetName(name);
		WidgetLogicFactoryPtr factory = MakeShareable(pFactory);
		WidgetLogicFactoryManager::GetInstance()->AddFactory(factory);
	}
};

#define WIDGET_LOGIC_FACTORY_REG(T) static WidgetLogicFactoryAutoRegister<T> WLFAR_##T##(TEXT(#T))
