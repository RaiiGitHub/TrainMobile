#pragma once

#include "WidgetLogicBase.h"

class WidgetLogicGameMenu :
	public WidgetLogicBase
{
public:
	WidgetLogicGameMenu();
	~WidgetLogicGameMenu();
protected:
public:
	DECLARE_GETBLUEPRINTCLASSPATH(WidgetLogicGameMenu);
	virtual void OnButtonClick(const FString ObjectName) override;
	virtual void OnConstruct() override;
	virtual void OnDestruct() override;

};


