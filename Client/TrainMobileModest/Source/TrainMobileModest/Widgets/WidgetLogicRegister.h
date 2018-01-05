#pragma once

#include "WidgetLogicBase.h"

class WidgetLogicRegister :
	public WidgetLogicBase
{
public:
	enum {REG_STEP1 = 1,REG_STEP2,REG_STEP3};
	WidgetLogicRegister();
	~WidgetLogicRegister();

public:
	bool VerifyAgreement();
	bool VerifyRegisterContent();
	void PutKeyValInClientUser();
	void SetupTextboxValue();
	void SetupTextboxHint();
	void GotoNextStep();
	void GotoPreStep();
	void SetTextBoxString(int nIndex,const FString& content);
	FString GetTextBoxString(int nIndex);//1,2,3

public:
	DECLARE_GETBLUEPRINTCLASSPATH(WidgetLogicRegister);
	virtual void OnButtonClick(const FString ObjectName) override;
	virtual void OnTextChanged(const FString ObjectName, const FText Text) override;
	virtual void OnConstruct() override;
	virtual void OnDestruct() override;

protected:
	int RegisterStep;

};


