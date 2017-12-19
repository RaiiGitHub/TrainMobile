#pragma once

#include "WidgetLogicBase.h"

class WidgetLogicLogin :
	public WidgetLogicBase
{
public:
	WidgetLogicLogin();
	~WidgetLogicLogin();
protected:
	void ReadServerAddr();
	void ReadUserLoginInfo();
	void ShowLogining(bool bShow);
	void SaveServerAddr();
	FString GetUserID();
	FString GetUserPsw();
	FString GetServerAddr();
	void    SetUserPsw(const FString& psw);
	bool PasswordMode(const FString& key, const FString& content,FString& psw);
	void StandaloneGameMode();
public:
	virtual void OnButtonClick(const FString ObjectName) override;
	virtual void OnTextChanged(const FString ObjectName, const FText Text) override;
	virtual void OnConstruct() override;
	virtual void OnDestruct() override;

};


