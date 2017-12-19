#pragma once

#include "WidgetLogicBase.h"

class WidgetLogicSimpleMessage :
	public WidgetLogicBase
{
public:
	WidgetLogicSimpleMessage();
	~WidgetLogicSimpleMessage();
protected:

public:
	virtual void OnButtonClick(const FString ObjectName) override;

protected:
	class UUserWidget* Caller;
};


