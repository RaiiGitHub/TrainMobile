#include "WidgetLogicRegister.h"
#include "TrainMobileModest.h"
#include "TrainGameInstance.h"
#include "Blueprint/UserWidget.h"
#include "GlobalValueContainer.h"
#include "WidgetLogicSimpleMessage.h"
#include "WidgetLogicLogin.h"
#include "UserWidgetWrapper.h"
#include "TrainOnline.h"

#include "Runtime/UMG/Public/UMG.h"
#include "Runtime/UMG/Public/UMGStyle.h"
#include "Runtime/UMG/Public/Slate/SObjectWidget.h"
#include "Runtime/UMG/Public/IUMGModule.h"
#include "Runtime/UMG/Public/Blueprint/UserWidget.h"

WIDGET_LOGIC_FACTORY_REG(WidgetLogicRegister);
IMPL_GETBLUEPRINTCLASSPATH(WidgetLogicRegister, WidgetBlueprint'/Game/Widget/UserRegister.UserRegister_C');

WidgetLogicRegister::WidgetLogicRegister()
	:RegisterStep(1)
{
}

WidgetLogicRegister::~WidgetLogicRegister()
{
}

bool WidgetLogicRegister::VerifyAgreement()
{
	if (nullptr == UTrainGameInstance::Instance || nullptr == UserWidget)
		return false;
	UCheckBox* pCheckBox = Cast<UCheckBox>(UserWidget->GetWidgetFromName(TEXT("cb_agree")));
	if (pCheckBox && pCheckBox->IsChecked())
		return true;
	
	//show message
	TSubclassOf<UUserWidget> WidgetClass = WidgetLogicBase::CreateUserWidgetClass(UserWidget, WidgetLogicSimpleMessage::BlueprintClassPath);
	UUserWidget* pUserWidget = UGlobalValueContainer::CreateUserWidget(WidgetClass);
	if (pUserWidget)
	{
		WidgetLogicSimpleMessage* pLogic = static_cast<WidgetLogicSimpleMessage*>(Cast<UUserWidgetWrapper>(pUserWidget)->GetWidgetLogic());
		if (pLogic)
		{
			pLogic->SetWidgetCaller(UserWidget);
			pLogic->SetTitle(UGlobalValueContainer::GetSystemValue(TEXT("msg.simple.register.step1.title")));
			pLogic->SetContent(UGlobalValueContainer::GetSystemValue(TEXT("msg.simple.register.step1.agreement.content")));
			pLogic->SetStyle(true, false);
		}
	}

	return false;
}

bool WidgetLogicRegister::VerifyRegisterContent()
{
	switch (RegisterStep)
	{
	case REG_STEP1:
	{
		//step1, account,password.
		FString user_id = GetTextBoxString(1);
		FString psw = UGlobalValueContainer::GetPswContent(TEXT("register_password"),false);
		FString psw_confirm = UGlobalValueContainer::GetPswContent(TEXT("register_password_repeat"), false);

		FString RegUserName(TEXT("^[a-zA-Z0-9_]{4,14}$"));
		FString RegPsw(TEXT("^[a-zA-Z0-9_]{6,14}$"));
		
		FRegexPattern PatternUserName(RegUserName);
		FRegexMatcher regMatcher(PatternUserName, user_id);
		regMatcher.SetLimits(0, user_id.Len());
		if (!regMatcher.FindNext())
		{
			TSubclassOf<UUserWidget> WidgetClass = WidgetLogicBase::CreateUserWidgetClass(UserWidget, WidgetLogicSimpleMessage::BlueprintClassPath);
			UUserWidget* pUserWidget = UGlobalValueContainer::CreateUserWidget(WidgetClass);
			if (pUserWidget)
			{
				WidgetLogicSimpleMessage* pLogic = static_cast<WidgetLogicSimpleMessage*>(Cast<UUserWidgetWrapper>(pUserWidget)->GetWidgetLogic());
				if (pLogic)
				{
					pLogic->SetWidgetCaller(UserWidget);
					pLogic->SetTitle(UGlobalValueContainer::GetSystemValue(TEXT("msg.simple.register.step1.title")));
					pLogic->SetContent(UGlobalValueContainer::GetSystemValue(TEXT("msg.simple.register.step1.username.content")));
					pLogic->SetStyle(true, false);
				}
			}
		}
		else
		{
			if (psw.Equals(psw_confirm))
			{
				FRegexPattern PatternPsw(RegPsw);
				FRegexMatcher regMatcherPsw(PatternPsw, psw);
				regMatcherPsw.SetLimits(0, psw.Len());
				if (regMatcherPsw.FindNext())
				{
					//need to verify whether user exist or not.
					UTrainGameInstance::Instance->UserExistVerify(user_id);
					return false;//in the callback.
				}
			}

			TSubclassOf<UUserWidget> WidgetClass = WidgetLogicBase::CreateUserWidgetClass(UserWidget, WidgetLogicSimpleMessage::BlueprintClassPath);
			UUserWidget* pUserWidget = UGlobalValueContainer::CreateUserWidget(WidgetClass);
			if (pUserWidget)
			{
				WidgetLogicSimpleMessage* pLogic = static_cast<WidgetLogicSimpleMessage*>(Cast<UUserWidgetWrapper>(pUserWidget)->GetWidgetLogic());
				if (pLogic)
				{
					pLogic->SetWidgetCaller(UserWidget);
					pLogic->SetTitle(UGlobalValueContainer::GetSystemValue(TEXT("msg.simple.register.step1.title")));
					pLogic->SetContent(UGlobalValueContainer::GetSystemValue(TEXT("msg.simple.register.step1.password.content")));
					pLogic->SetStyle(true, false);
				}
			}

		}
	}
		break;
	case REG_STEP2:
	{
		//step2, name, phone.no, id.
		FString user_name = GetTextBoxString(1);
		FString phone_no = GetTextBoxString(2);
		FString id_no = GetTextBoxString(3);
		if (user_name.IsEmpty() || phone_no.IsEmpty() || id_no.IsEmpty())
		{
			TSubclassOf<UUserWidget> WidgetClass = WidgetLogicBase::CreateUserWidgetClass(UserWidget, WidgetLogicSimpleMessage::BlueprintClassPath);
			UUserWidget* pUserWidget = UGlobalValueContainer::CreateUserWidget(WidgetClass);
			if (pUserWidget)
			{
				WidgetLogicSimpleMessage* pLogic = static_cast<WidgetLogicSimpleMessage*>(Cast<UUserWidgetWrapper>(pUserWidget)->GetWidgetLogic());
				if (pLogic)
				{
					pLogic->SetWidgetCaller(UserWidget);
					pLogic->SetTitle(UGlobalValueContainer::GetSystemValue(TEXT("msg.simple.register.step2.title")));
					pLogic->SetContent(UGlobalValueContainer::GetSystemValue(TEXT("msg.simple.register.notfull.content")));
					pLogic->SetStyle(true, false);
				}
			}
		}
		else
		{
			FString RegPhoneNo(TEXT("^[0-9]{11}$"));

			FRegexPattern Pattern(RegPhoneNo);
			FRegexMatcher regMatcher(Pattern, phone_no);

			regMatcher.SetLimits(0, phone_no.Len());
			if (!regMatcher.FindNext())
			{
				TSubclassOf<UUserWidget> WidgetClass = WidgetLogicBase::CreateUserWidgetClass(UserWidget, WidgetLogicSimpleMessage::BlueprintClassPath);
				UUserWidget* pUserWidget = UGlobalValueContainer::CreateUserWidget(WidgetClass);
				if (pUserWidget)
				{
					WidgetLogicSimpleMessage* pLogic = static_cast<WidgetLogicSimpleMessage*>(Cast<UUserWidgetWrapper>(pUserWidget)->GetWidgetLogic());
					if (pLogic)
					{
						pLogic->SetWidgetCaller(UserWidget);
						pLogic->SetTitle(UGlobalValueContainer::GetSystemValue(TEXT("msg.simple.register.step2.title")));
						pLogic->SetContent(UGlobalValueContainer::GetSystemValue(TEXT("msg.simple.register.invalid.phone.content")));
						pLogic->SetStyle(true, false);
					}
				}
			}
			else
			{
				return true;
			}	
		}
	}
		break;
	case REG_STEP3:
	{
		//step3, school, college, adress.
		FString school  = GetTextBoxString(1);
		FString college = GetTextBoxString(2);
		FString adress  = GetTextBoxString(3);
		if (school.IsEmpty() || college.IsEmpty() || adress.IsEmpty())
		{
			TSubclassOf<UUserWidget> WidgetClass = WidgetLogicBase::CreateUserWidgetClass(UserWidget, WidgetLogicSimpleMessage::BlueprintClassPath);
			UUserWidget* pUserWidget = UGlobalValueContainer::CreateUserWidget(WidgetClass);
			if (pUserWidget)
			{
				WidgetLogicSimpleMessage* pLogic = static_cast<WidgetLogicSimpleMessage*>(Cast<UUserWidgetWrapper>(pUserWidget)->GetWidgetLogic());
				if (pLogic)
				{
					pLogic->SetWidgetCaller(UserWidget);
					pLogic->SetTitle(UGlobalValueContainer::GetSystemValue(TEXT("msg.simple.register.step3.title")));
					pLogic->SetContent(UGlobalValueContainer::GetSystemValue(TEXT("msg.simple.register.notfull.content")));
					pLogic->SetStyle(true, false);
				}
			}
		}
		else
		{
			return true;
		}
	}
		break;
	}
	return false;
}

void WidgetLogicRegister::PutKeyValInClientUser()
{
	if (nullptr == UTrainGameInstance::Instance || nullptr == UserWidget)
		return;
	FString val1 = GetTextBoxString(1);
	FString val2 = GetTextBoxString(2);
	FString val3 = GetTextBoxString(3);
	if (REG_STEP1 == RegisterStep)
	{
		val2 = UGlobalValueContainer::GetPswContent(TEXT("register_password"), false);
		UTrainGameInstance::Instance->PutKeyValInPendingUser(TEXT("id"),val1);
		UTrainGameInstance::Instance->PutKeyValInPendingUser(TEXT("psw"),val2);
	}
	else if (REG_STEP2 == RegisterStep)
	{
		UTrainGameInstance::Instance->PutKeyValInPendingUser(TEXT("name"), val1);
		UTrainGameInstance::Instance->PutKeyValInPendingUser(TEXT("phone_number"), val2);
		UTrainGameInstance::Instance->PutKeyValInPendingUser(TEXT("identity_number"), val3);
	}
	else if (REG_STEP3 == RegisterStep)
	{
		UTrainGameInstance::Instance->PutKeyValInPendingUser(TEXT("train_school"), val1);
		UTrainGameInstance::Instance->PutKeyValInPendingUser(TEXT("institution"), val2);
		UTrainGameInstance::Instance->PutKeyValInPendingUser(TEXT("address"), val3);
	}
}

void WidgetLogicRegister::SetupTextboxValue()
{
	UEditableTextBox* pText1 = Cast<UEditableTextBox>(UserWidget->GetWidgetFromName(TEXT("tb_1")));
	UEditableTextBox* pText2 = Cast<UEditableTextBox>(UserWidget->GetWidgetFromName(TEXT("tb_2")));
	UEditableTextBox* pText3 = Cast<UEditableTextBox>(UserWidget->GetWidgetFromName(TEXT("tb_3")));
	if (pText1 && pText2 && pText3)
	{
		const TSharedPtr<SUser>& u = GetUserProducer()->GetClientUser();
		switch (RegisterStep)
		{
		case REG_STEP1:
			pText1->SetText(FText::FromString(u->id_));
			pText2->SetText(FText::FromString(u->psw_));
			pText3->SetText(FText::FromString(u->psw_));
			break;
		case REG_STEP2:
			pText1->SetText(FText::FromString(u->name_));
			pText2->SetText(FText::FromString(u->other_field_value_.FindOrAdd(TEXT("phone_number"))));
			pText3->SetText(FText::FromString(u->other_field_value_.FindOrAdd(TEXT("identity_number"))));
			break;
		case REG_STEP3:
			pText1->SetText(FText::FromString(u->other_field_value_.FindOrAdd(TEXT("train_school"))));
			pText2->SetText(FText::FromString(u->other_field_value_.FindOrAdd(TEXT("institution"))));
			pText3->SetText(FText::FromString(u->other_field_value_.FindOrAdd(TEXT("address"))));
			break;
		}
	}
}

void WidgetLogicRegister::SetupTextboxHint()
{
	UEditableTextBox* pText1 = Cast<UEditableTextBox>(UserWidget->GetWidgetFromName(TEXT("tb_1")));
	UEditableTextBox* pText2 = Cast<UEditableTextBox>(UserWidget->GetWidgetFromName(TEXT("tb_2")));
	UEditableTextBox* pText3 = Cast<UEditableTextBox>(UserWidget->GetWidgetFromName(TEXT("tb_3")));
	UTextBlock*       pBlockBtnLeft = Cast<UTextBlock>(UserWidget->GetWidgetFromName(TEXT("cancel_text")));
	UTextBlock*       pBlockTitle   = Cast<UTextBlock>(UserWidget->GetWidgetFromName(TEXT("title")));

	if (pText1 && pText2 && pText3 && pBlockBtnLeft && pBlockTitle)
	{
		switch (RegisterStep)
		{
		case REG_STEP1:
			pBlockBtnLeft->SetText(FText::FromString(UGlobalValueContainer::GetSystemValue(TEXT("register.textbox.hint.cancel"))));
			pBlockTitle->SetText(FText::FromString(UGlobalValueContainer::GetSystemValue(TEXT("register.textbox.hint.step1.title"))));
			pText1->SetHintText(FText::FromString(UGlobalValueContainer::GetSystemValue(TEXT("register.textbox.hint.step1.text1"))));
			pText2->SetHintText(FText::FromString(UGlobalValueContainer::GetSystemValue(TEXT("register.textbox.hint.step1.text2"))));
			pText3->SetHintText(FText::FromString(UGlobalValueContainer::GetSystemValue(TEXT("register.textbox.hint.step1.text3"))));
			break;
		case REG_STEP2:
			pBlockBtnLeft->SetText(FText::FromString(UGlobalValueContainer::GetSystemValue(TEXT("register.textbox.hint.back"))));
			pBlockTitle->SetText(FText::FromString(UGlobalValueContainer::GetSystemValue(TEXT("register.textbox.hint.step2.title"))));
			pText1->SetHintText(FText::FromString(UGlobalValueContainer::GetSystemValue(TEXT("register.textbox.hint.step2.text1"))));
			pText2->SetHintText(FText::FromString(UGlobalValueContainer::GetSystemValue(TEXT("register.textbox.hint.step2.text2"))));
			pText3->SetHintText(FText::FromString(UGlobalValueContainer::GetSystemValue(TEXT("register.textbox.hint.step2.text3"))));
			break;
		case REG_STEP3:
			pBlockBtnLeft->SetText(FText::FromString(UGlobalValueContainer::GetSystemValue(TEXT("register.textbox.hint.back"))));
			pBlockTitle->SetText(FText::FromString(UGlobalValueContainer::GetSystemValue(TEXT("register.textbox.hint.step3.title"))));
			pText1->SetHintText(FText::FromString(UGlobalValueContainer::GetSystemValue(TEXT("register.textbox.hint.step3.text1"))));
			pText2->SetHintText(FText::FromString(UGlobalValueContainer::GetSystemValue(TEXT("register.textbox.hint.step3.text2"))));
			pText3->SetHintText(FText::FromString(UGlobalValueContainer::GetSystemValue(TEXT("register.textbox.hint.step3.text3"))));
			break;
		}
	}
}

void WidgetLogicRegister::GotoNextStep()
{
	if (REG_STEP1 == RegisterStep)
	{
		//will goto step2.
		PutKeyValInClientUser();
		RegisterStep = REG_STEP2;
		SetupTextboxValue();//read step2.
		SetupTextboxHint();
	}
	else if (REG_STEP2 == RegisterStep)
	{
		//will goto step3
		PutKeyValInClientUser();
		RegisterStep = REG_STEP3;
		SetupTextboxValue();//read step3.
		SetupTextboxHint();
	}
	else if (REG_STEP3 == RegisterStep)
	{
		//will be end.
		PutKeyValInClientUser();
		UTrainGameInstance::Instance->UserRegister();
		//return.
		UButton* pBlockBtnLeft  = Cast<UButton>(UserWidget->GetWidgetFromName(TEXT("next_step")));
		UButton* pBlockBtnRight = Cast<UButton>(UserWidget->GetWidgetFromName(TEXT("cancel")));
		if (pBlockBtnLeft && pBlockBtnRight)
		{
			pBlockBtnLeft->SetIsEnabled(false);
			pBlockBtnRight->SetIsEnabled(false);
		}
	}
}

void WidgetLogicRegister::GotoPreStep()
{
	if (REG_STEP1 == RegisterStep)
	{
		//will goback to login page.
		TSubclassOf<UUserWidget> WidgetClass = WidgetLogicBase::CreateUserWidgetClass(UserWidget, WidgetLogicLogin::BlueprintClassPath);
		UUserWidget* pUserWidget = UGlobalValueContainer::CreateUserWidget(WidgetClass);
		if (pUserWidget)
		{
			WidgetLogicLogin* pLogic = static_cast<WidgetLogicLogin*>(Cast<UUserWidgetWrapper>(pUserWidget)->GetWidgetLogic());
			if (pLogic)
			{
				//may fill something...
			}
		}
		UGlobalValueContainer::RemoveWidgetFromViewport(UserWidget);//remove self.
	}
	else if (REG_STEP2 == RegisterStep)
	{
		//will goto step1
		RegisterStep = REG_STEP1;
		SetupTextboxValue();//read step1.
		SetupTextboxHint();
	}
	else if (REG_STEP3 == RegisterStep)
	{
		//will goto step2
		RegisterStep = REG_STEP2;
		SetupTextboxValue();//read step2.
		SetupTextboxHint();
	}
}

void WidgetLogicRegister::SetTextBoxString(int nIndex, const FString & content)
{
	if (nullptr == UTrainGameInstance::Instance || nullptr == UserWidget)
		return;
	FName name(*FString::Printf(TEXT("tb_%d"), nIndex));
	UEditableTextBox* pText = Cast<UEditableTextBox>(UserWidget->GetWidgetFromName(name));
	if (pText)
		return pText->SetText(FText::FromString(content));
}

FString WidgetLogicRegister::GetTextBoxString(int nIndex)
{
	if (nullptr == UTrainGameInstance::Instance || nullptr == UserWidget)
		return FString();

	FName name(*FString::Printf(TEXT("tb_%d"), nIndex));
	UEditableTextBox* pText = Cast<UEditableTextBox>(UserWidget->GetWidgetFromName(name));
	if (pText)
		return pText->GetText().ToString();

	return FString();
}



void WidgetLogicRegister::OnConstruct()
{

}

void WidgetLogicRegister::OnDestruct()
{
	UGlobalValueContainer::RemovePswContent(TEXT("register_password"));
	UGlobalValueContainer::RemovePswContent(TEXT("register_password_repeat"));
}

void WidgetLogicRegister::OnButtonClick(const FString ObjectName)
{
	if (nullptr == UTrainGameInstance::Instance || nullptr == UserWidget)
		return;
	if (ObjectName.Equals(TEXT("next_step"), ESearchCase::IgnoreCase))
	{
		if (VerifyAgreement() && VerifyRegisterContent() )
		{
			GotoNextStep();
		}
	}
	else if (ObjectName.Equals(TEXT("cancel"), ESearchCase::IgnoreCase))
	{
		GotoPreStep();
	}
}

void WidgetLogicRegister::OnTextChanged(const FString ObjectName, const FText Text)
{
	if (REG_STEP1 == RegisterStep)
	{
		if (ObjectName.Equals(TEXT("tb_2"), ESearchCase::IgnoreCase))
		{
			if (UGlobalValueContainer::AddPswContent(TEXT("register_password"), Text.ToString()))
				SetTextBoxString(2, UGlobalValueContainer::GetPswContent(TEXT("register_password"), true));
		}
		else if (ObjectName.Equals(TEXT("tb_3"), ESearchCase::IgnoreCase))
		{
			if (UGlobalValueContainer::AddPswContent(TEXT("register_password_repeat"), Text.ToString()))
				SetTextBoxString(3, UGlobalValueContainer::GetPswContent(TEXT("register_password_repeat"), true));
		}
	}
}


