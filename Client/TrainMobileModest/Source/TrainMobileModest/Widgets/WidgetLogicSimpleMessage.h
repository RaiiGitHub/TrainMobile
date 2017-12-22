#pragma once

#include "WidgetLogicBase.h"

class WidgetLogicSimpleMessage :
	public WidgetLogicBase
{
public:
	WidgetLogicSimpleMessage();
	~WidgetLogicSimpleMessage();
public:
	DECLARE_GETBLUEPRINTCLASSPATH(WidgetLogicSimpleMessage);
	void SetWidgetCaller(UUserWidget* pWidget);
	void SetTitle(const FString& title);
	void SetContent(const FString& content);
	void SetStyle(bool bContainOkButton, bool bContainCancelButton);
public:
	virtual void OnButtonClick(const FString ObjectName) override;
	virtual void OnConstruct() override;
	virtual void OnDestruct() override;
protected:
	class UUserWidget* Caller;

};


