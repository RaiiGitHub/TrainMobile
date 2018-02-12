#pragma once

#include "WidgetLogicBase.h"

class WidgetLogicReturn :
	public WidgetLogicBase
{
public:
	WidgetLogicReturn();
	~WidgetLogicReturn();
protected:
public:
	DECLARE_GETBLUEPRINTCLASSPATH(WidgetLogicReturn);
	virtual void OnButtonClick(const FString ObjectName) override;
	virtual void OnConstruct() override;
	virtual void OnDestruct() override;

};


