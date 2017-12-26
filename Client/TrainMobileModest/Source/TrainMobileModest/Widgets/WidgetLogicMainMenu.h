#pragma once

#include "WidgetLogicBase.h"

class WidgetLogicMainMenu :
	public WidgetLogicBase
{
public:
	WidgetLogicMainMenu();
	~WidgetLogicMainMenu();
protected:
	void UpdateUserLabel();
public:
	DECLARE_GETBLUEPRINTCLASSPATH(WidgetLogicMainMenu);
	virtual void OnButtonClick(const FString ObjectName) override;
	virtual void OnConstruct() override;
	virtual void OnDestruct() override;

};


