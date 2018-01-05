#include "TrainMobileModest.h"
#include "WidgetLogicLogin.h"
#include "TrainGameInstance.h"
#include "Blueprint/UserWidget.h"
#include "GlobalValueContainer.h"
#include "WidgetLogicSimpleMessage.h"
#include "UserWidgetWrapper.h"
#include "Runtime/UMG/Public/UMG.h"
#include "Runtime/UMG/Public/UMGStyle.h"
#include "Runtime/UMG/Public/Slate/SObjectWidget.h"
#include "Runtime/UMG/Public/IUMGModule.h"
#include "Runtime/UMG/Public/Blueprint/UserWidget.h"

WIDGET_LOGIC_FACTORY_REG(WidgetLogicLogin);
IMPL_GETBLUEPRINTCLASSPATH(WidgetLogicLogin, WidgetBlueprint'/Game/Widget/UserLogin.UserLogin_C');

WidgetLogicLogin::WidgetLogicLogin()
{
}

WidgetLogicLogin::~WidgetLogicLogin()
{
}

void WidgetLogicLogin::ReadServerAddr()
{
	if (nullptr == UTrainGameInstance::Instance || nullptr == UserWidget)
		return;

	UTrainGameInstance::Instance->WriteConfig(TEXT("InputKeyboard"), TEXT("Intergrated"), TEXT("-1"));
	FString server_addr = UTrainGameInstance::Instance->ReadConfig(TEXT("Network.HttpServer"),TEXT("Addr"));
	if (server_addr.IsEmpty())
		server_addr = UGlobalValueContainer::GetSystemValue(TEXT("DefaultServerAddr"));
	UEditableTextBox* pText = Cast<UEditableTextBox>(UserWidget->GetWidgetFromName(TEXT("tb_server")));
	if (pText)
		pText->SetText(FText::FromString(server_addr));
}

void WidgetLogicLogin::ReadUserLoginInfo()
{
	if (nullptr == UTrainGameInstance::Instance || nullptr == UserWidget)
		return;
	FString client_id = UTrainGameInstance::Instance->ReadConfig(TEXT("Network.Client"), TEXT("ID"));
	{
		UEditableTextBox* pText = Cast<UEditableTextBox>(UserWidget->GetWidgetFromName(TEXT("tb_account")));
		if (pText)
			pText->SetText(FText::FromString(client_id));
	}
	FString client_psw = UTrainGameInstance::Instance->ReadConfig(TEXT("Network.Client"), TEXT("Password"));
	{
		if (UGlobalValueContainer::AddPswContent(TEXT("login_password"), client_psw))
			SetUserPsw(UGlobalValueContainer::GetPswContent(TEXT("login_password"),true));
	}
}

void WidgetLogicLogin::ShowLogining(bool bShow)
{
	if (nullptr == UTrainGameInstance::Instance || nullptr == UserWidget)
		return;
	UWidget* pWidget = UserWidget->GetWidgetFromName(TEXT("login_status"));
	if (pWidget)
		pWidget->SetVisibility(bShow?ESlateVisibility::Visible:ESlateVisibility::Hidden);
}

void WidgetLogicLogin::EnableLoginButton(bool bEnable)
{
	UButton* pButton = Cast<UButton>(UserWidget->GetWidgetFromName(TEXT("login")));
	if (pButton)
		pButton->SetIsEnabled(bEnable);
}

void WidgetLogicLogin::SaveServerAddr()
{
	if (nullptr == UTrainGameInstance::Instance || nullptr == UserWidget)
		return;
	UEditableTextBox* pText = Cast<UEditableTextBox>(UserWidget->GetWidgetFromName(TEXT("tb_server")));
	if (pText)
	{
		FString server_addr = pText->GetText().ToString();
		UTrainGameInstance::Instance->WriteConfig(TEXT("Network.HttpServer"), TEXT("Addr"), server_addr);
	}
}

FString WidgetLogicLogin::GetUserID()
{
	UEditableTextBox* pText = Cast<UEditableTextBox>(UserWidget->GetWidgetFromName(TEXT("tb_account")));
	if (pText)
		return pText->GetText().ToString();
	return FString();
}

FString WidgetLogicLogin::GetUserPsw()
{
	return UGlobalValueContainer::GetPswContent(TEXT("login_password"), false);
}

FString WidgetLogicLogin::GetServerAddr()
{
	UEditableTextBox* pText = Cast<UEditableTextBox>(UserWidget->GetWidgetFromName(TEXT("tb_server")));
	if (pText)
	{
		FString server_addr = pText->GetText().ToString();
		if (server_addr.IsEmpty()
			|| server_addr.Equals(UGlobalValueContainer::GetSystemValue(TEXT("DefaultServerAddr"))))
		{
			//return TEXT("http://www.resafety.com/mobile/");
			return TEXT("http://127.0.0.1:8194/");
		}
		return server_addr;
	}
	return FString();
}

void WidgetLogicLogin::SetUserID(const FString & id)
{
	UEditableTextBox* pText = Cast<UEditableTextBox>(UserWidget->GetWidgetFromName(TEXT("tb_account")));
	if (pText)
		pText->SetText(FText::FromString(id));
}

void WidgetLogicLogin::SetUserPsw(const FString & psw)
{
	UEditableTextBox* pText = Cast<UEditableTextBox>(UserWidget->GetWidgetFromName(TEXT("tb_psw")));
	if (pText)
		pText->SetText(FText::FromString(psw));
}

void WidgetLogicLogin::EnterMainMenu()
{
	if (nullptr == UTrainGameInstance::Instance || nullptr == UserWidget)
		return;
	UGlobalValueContainer::RemoveWidgetFromViewport(UserWidget);
	//create mainmenu widget.
}

void WidgetLogicLogin::StandaloneGameMode()
{
	if (nullptr == UTrainGameInstance::Instance || nullptr == UserWidget)
		return;
	UTrainGameInstance::Instance->SetOfflineMode();
	//showmessagesimple.

	TSubclassOf<UUserWidget> WidgetClass = WidgetLogicBase::CreateUserWidgetClass(UserWidget, WidgetLogicSimpleMessage::BlueprintClassPath);
	UUserWidget* pUserWidget = UGlobalValueContainer::CreateUserWidget(WidgetClass);
	if (pUserWidget)
	{
		WidgetLogicSimpleMessage* pLogic = static_cast<WidgetLogicSimpleMessage*>(Cast<UUserWidgetWrapper>(pUserWidget)->GetWidgetLogic());
		if (pLogic)
		{
			pLogic->SetWidgetCaller(UserWidget);
			pLogic->SetTitle(UGlobalValueContainer::GetSystemValue(TEXT("msg.simple.login.standalone.title")));
			pLogic->SetContent(UGlobalValueContainer::GetSystemValue(TEXT("msg.simple.login.standalone.content")));
			pLogic->SetStyle(true, true);
		}
	}
}

void WidgetLogicLogin::OnConstruct()
{
	ReadServerAddr();
	ReadUserLoginInfo();
	ShowLogining(false);
}

void WidgetLogicLogin::OnDestruct()
{
	UGlobalValueContainer::RemovePswContent(TEXT("login_password"));
}

void WidgetLogicLogin::OnButtonClick(const FString ObjectName)
{
	if (nullptr == UTrainGameInstance::Instance || nullptr == UserWidget)
		return;
	if (ObjectName.Equals(TEXT("login"), ESearchCase::IgnoreCase))
	{
		ShowLogining(true);
		EnableLoginButton(false);
		SaveServerAddr();
		if (GetUserID().Equals(TEXT("su"), ESearchCase::IgnoreCase))
		{
			StandaloneGameMode();
		}
		else
		{
			UTrainGameInstance::Instance->ServerInit(GetServerAddr());
			UTrainGameInstance::Instance->UserLogin(GetUserID(), GetUserPsw());
		}
	}
	else if (ObjectName.Equals(TEXT("register"), ESearchCase::IgnoreCase))
	{
		ShowLogining(true);
		EnableLoginButton(false);
		UTrainGameInstance::Instance->ServerInit(GetServerAddr());
		UTrainGameInstance::Instance->Handshake();
	}
	else if (ObjectName.Equals(TEXT("ok"), ESearchCase::IgnoreCase))
	{
		//navigate to the main widget.
		EnterMainMenu();
	}
}

void WidgetLogicLogin::OnTextChanged(const FString ObjectName, const FText Text)
{
	if (ObjectName.Equals(TEXT("tb_psw"), ESearchCase::IgnoreCase))
	{
		if (UGlobalValueContainer::AddPswContent(TEXT("login_password"), Text.ToString()))
			SetUserPsw(UGlobalValueContainer::GetPswContent(TEXT("login_password"),true));
	}
}


