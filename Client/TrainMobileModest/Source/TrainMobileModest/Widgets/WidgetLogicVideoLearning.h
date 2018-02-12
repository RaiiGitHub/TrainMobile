#pragma once

#include "WidgetLogicBase.h"

class WidgetLogicVideoLearning :
	public WidgetLogicBase
{
public:
	WidgetLogicVideoLearning();
	~WidgetLogicVideoLearning();
protected:
public:
	DECLARE_GETBLUEPRINTCLASSPATH(WidgetLogicVideoLearning);
	virtual void OnButtonClick(const FString ObjectName) override;
	virtual void OnConstruct() override;
	virtual void OnDestruct() override;

};


