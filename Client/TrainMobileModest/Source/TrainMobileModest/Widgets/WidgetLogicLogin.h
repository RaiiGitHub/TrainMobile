#pragma once

#include "WidgetLogicBase.h"

class WidgetLogicLogin :
	public WidgetLogicBase
{
public:
	WidgetLogicLogin();
	~WidgetLogicLogin();
protected:
	void    ReadServerAddr();
	void    ReadUserLoginInfo();
	void    SaveServerAddr();
	FString GetUserID();
	FString GetUserPsw();
	FString GetServerAddr();
	void    EnterMainMenu();
	void    StandaloneGameMode();
public:
	void    SetUserID(const FString& id);
	void    SetUserPsw(const FString& psw);
	void    ShowLogining(bool bShow);
	void    EnableLoginButton(bool bEnable);
public:
	DECLARE_GETBLUEPRINTCLASSPATH(WidgetLogicLogin);
	virtual void OnButtonClick(const FString ObjectName) override;
	virtual void OnTextChanged(const FString ObjectName, const FText Text) override;
	virtual void OnConstruct() override;
	virtual void OnDestruct() override;

};


