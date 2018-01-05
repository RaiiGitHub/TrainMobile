// As Part of GuangZhou Training.

#include "TrainMobileModest.h"
#include "TrainGameInstance.h"
#include "TaskPool.h"
#include "CallbackActor.h"
#include "TrainOnline.h"
#include "LuaProvider.h"
#include "sprotodef.h"
#include "HumanCharacter.h"

#include "GlobalValueContainer.h"
#include "Blueprint/UserWidget.h"
#include "Widgets/WidgetLogicSimpleMessage.h"
#include "Widgets/WidgetLogicLogin.h"
#include "Widgets/WidgetLogicMainMenu.h"
#include "Widgets/WidgetLogicRegister.h"
#include "Widgets/UserWidgetWrapper.h"

#include "Runtime/WebBrowser/Public/IWebBrowserSingleton.h"
#include "Runtime/WebBrowser/Public/WebBrowserModule.h"
#include "Runtime/WebBrowser/Public/IWebBrowserCookieManager.h"

DECLARE_LOG_CATEGORY_CLASS(RaiiGameInstance, Log, All);

UTrainGameInstance* UTrainGameInstance::Instance = nullptr;
static TMap<FString, EMemberStage> MEMBER_STAGE;
static TMap<EMemberStage, FString> MEMBER_STAGE_REVERSE;
static TMap<FString, ELobbyStage> LSM;
static FString LobbyFinishMark(TEXT("lobby:role_select_finished"));
static int MatchJoinTries = 0;

////////interface of trainonline begin////////////
void UTrainGameInstance::NotifyHttpRequestErr(const FString& msg)
{
	//pop errors
	TSubclassOf<UUserWidget> WidgetClass = WidgetLogicBase::CreateUserWidgetClass(this, WidgetLogicSimpleMessage::BlueprintClassPath);
	UUserWidget* pUserWidget = UGlobalValueContainer::CreateUserWidget(WidgetClass);
	if (pUserWidget)
	{
		WidgetLogicSimpleMessage* pLogic = static_cast<WidgetLogicSimpleMessage*>(Cast<UUserWidgetWrapper>(pUserWidget)->GetWidgetLogic());
		if (pLogic)
		{
			pLogic->SetWidgetCaller(nullptr);
			pLogic->SetTitle(UGlobalValueContainer::GetSystemValue(TEXT("msg.simple.http.error.title")));
			pLogic->SetContent(msg);
			pLogic->SetStyle(true,false);
		}
	}
}

void UTrainGameInstance::NotifyLoginResponse(LoginStatus ls)
{
	if (ls == LoginStatus::LSP_FAILED)
	{
		//pop errors
		TSubclassOf<UUserWidget> WidgetClass = WidgetLogicBase::CreateUserWidgetClass(this, WidgetLogicSimpleMessage::BlueprintClassPath);
		UUserWidget* pUserWidget = UGlobalValueContainer::CreateUserWidget(WidgetClass);
		if (pUserWidget)
		{
			WidgetLogicSimpleMessage* pLogic = static_cast<WidgetLogicSimpleMessage*>(Cast<UUserWidgetWrapper>(pUserWidget)->GetWidgetLogic());
			if (pLogic)
			{
				pLogic->SetWidgetCaller(nullptr);
				pLogic->SetTitle(UGlobalValueContainer::GetSystemValue(TEXT("msg.simple.login.response.title")));
				pLogic->SetContent(UGlobalValueContainer::GetSystemValue(TEXT("msg.simple.login.response.failed.content")));
				pLogic->SetStyle(true, false);
			}
		}
		UUserWidget* pLoginWidget = UGlobalValueContainer::GetWidgetFromViewport(TEXT("WidgetLogicLogin"));
		if (pLoginWidget)
		{
			WidgetLogicLogin* pLogic = static_cast<WidgetLogicLogin*>(Cast<UUserWidgetWrapper>(pLoginWidget)->GetWidgetLogic());
			if (pLogic)
			{
				pLogic->EnableLoginButton(true);
				pLogic->ShowLogining(false);
			}
		}
	}
	else if( ls == LoginStatus::LSP_TOTAL )
	{
		//success, pop main menu and verify edition.
		UGlobalValueContainer::RemoveAllWidgetsFromViewport();
		TSubclassOf<UUserWidget> WidgetClass = WidgetLogicBase::CreateUserWidgetClass(this, WidgetLogicMainMenu::BlueprintClassPath);
		UUserWidget* pUserWidget = UGlobalValueContainer::CreateUserWidget(WidgetClass);
		if (pUserWidget)
		{
			VersionVerify();
		}
	}
}

void UTrainGameInstance::NotifyUserRankDataReady()
{

}

void UTrainGameInstance::NotifyUserIDExistVerify(const FString& id, bool exist)
{
	if (exist)
	{
		//invalid user_id
		TSubclassOf<UUserWidget> WidgetClass = WidgetLogicBase::CreateUserWidgetClass(this, WidgetLogicSimpleMessage::BlueprintClassPath);
		UUserWidget* pUserWidget = UGlobalValueContainer::CreateUserWidget(WidgetClass);
		if (pUserWidget)
		{
			WidgetLogicSimpleMessage* pLogic = static_cast<WidgetLogicSimpleMessage*>(Cast<UUserWidgetWrapper>(pUserWidget)->GetWidgetLogic());
			if (pLogic)
			{
				pLogic->SetWidgetCaller(nullptr);
				pLogic->SetTitle(UGlobalValueContainer::GetSystemValue(TEXT("msg.simple.register.step1.title")));
				pLogic->SetContent(UGlobalValueContainer::GetSystemValue(TEXT("msg.simple.register.userid.exist.content")) + id);
				pLogic->SetStyle(true, false);
			}
		}
	}
	else
	{
		//userregister widget. goto next page.
		UUserWidgetWrapper* pLoginWidget = Cast<UUserWidgetWrapper>(UGlobalValueContainer::GetWidgetFromViewport(TEXT("WidgetLogicRegister")));
		if (pLoginWidget)
		{
			WidgetLogicRegister* pLogic = static_cast<WidgetLogicRegister*>(pLoginWidget->GetWidgetLogic());
			if (pLogic)
				pLogic->GotoNextStep();
		}
	}
}

void UTrainGameInstance::NotifyHandshake(const FString& msg, bool shake)
{
	if (shake)
	{
		//ready to open userregister widget.
		TSubclassOf<UUserWidget> WidgetClass = WidgetLogicBase::CreateUserWidgetClass(this, WidgetLogicRegister::BlueprintClassPath);
		UUserWidget* pUserWidget = UGlobalValueContainer::CreateUserWidget(WidgetClass);
		if (pUserWidget)
		{
			WidgetLogicRegister* pLogic = static_cast<WidgetLogicRegister*>(Cast<UUserWidgetWrapper>(pUserWidget)->GetWidgetLogic());
			if (pLogic)
			{
				//
			}
		}
		UUserWidget* pLoginWidget = UGlobalValueContainer::GetWidgetFromViewport(TEXT("WidgetLogicLogin"));
		UGlobalValueContainer::RemoveWidgetFromViewport(pLoginWidget);
	}
	else
	{
		//failed to connect.
		//pop errors
		TSubclassOf<UUserWidget> WidgetClass = WidgetLogicBase::CreateUserWidgetClass(this, WidgetLogicSimpleMessage::BlueprintClassPath);
		UUserWidget* pUserWidget = UGlobalValueContainer::CreateUserWidget(WidgetClass);
		if (pUserWidget)
		{
			WidgetLogicSimpleMessage* pLogic = static_cast<WidgetLogicSimpleMessage*>(Cast<UUserWidgetWrapper>(pUserWidget)->GetWidgetLogic());
			if (pLogic)
			{
				pLogic->SetWidgetCaller(nullptr);
				pLogic->SetTitle(UGlobalValueContainer::GetSystemValue(TEXT("msg.simple.login.register.response.title")));
				pLogic->SetContent(UGlobalValueContainer::GetSystemValue(TEXT("msg.simple.login.register.response.failed.content")));
				pLogic->SetStyle(true, false);
			}
		}

		UUserWidget* pLoginWidget = UGlobalValueContainer::GetWidgetFromViewport(TEXT("WidgetLogicLogin"));
		if (pLoginWidget)
		{
			WidgetLogicLogin* pLogic = static_cast<WidgetLogicLogin*>(Cast<UUserWidgetWrapper>(pLoginWidget)->GetWidgetLogic());
			if (pLogic)
			{
				pLogic->EnableLoginButton(true);
				pLogic->ShowLogining(false);
			}
		}
	}
}

void UTrainGameInstance::NotifyRegisterResponse(bool success)
{
	if (success)
	{
		//register done.
		//ready to open userlogin widget.
		{
			TSubclassOf<UUserWidget> WidgetClass = WidgetLogicBase::CreateUserWidgetClass(this, WidgetLogicLogin::BlueprintClassPath);
			UUserWidget* pUserWidget = UGlobalValueContainer::CreateUserWidget(WidgetClass);
			if (pUserWidget)
			{
				WidgetLogicLogin* pLogic = static_cast<WidgetLogicLogin*>(Cast<UUserWidgetWrapper>(pUserWidget)->GetWidgetLogic());
				if (pLogic)
				{
					const TSharedPtr<SUser>& u = GetUserProducer()->GetClientUser();
					pLogic->SetUserID(u->id_);
					if (UGlobalValueContainer::AddPswContent(TEXT("login_password"), u->psw_))
						pLogic->SetUserPsw(UGlobalValueContainer::GetPswContent(TEXT("login_password"), true));
				}
			}
			UUserWidget* pRegisterWidget = UGlobalValueContainer::GetWidgetFromViewport(TEXT("WidgetLogicRegister"));
			UGlobalValueContainer::RemoveWidgetFromViewport(pRegisterWidget);
		}

		//tips
		{
			TSubclassOf<UUserWidget> WidgetClass = WidgetLogicBase::CreateUserWidgetClass(this, WidgetLogicSimpleMessage::BlueprintClassPath);
			UUserWidget* pUserWidget = UGlobalValueContainer::CreateUserWidget(WidgetClass);
			if (pUserWidget)
			{
				WidgetLogicSimpleMessage* pLogic = static_cast<WidgetLogicSimpleMessage*>(Cast<UUserWidgetWrapper>(pUserWidget)->GetWidgetLogic());
				if (pLogic)
				{
					pLogic->SetWidgetCaller(nullptr);
					pLogic->SetTitle(UGlobalValueContainer::GetSystemValue(TEXT("msg.simple.register.success.response.title")));
					pLogic->SetContent(UGlobalValueContainer::GetSystemValue(TEXT("msg.simple.register.success.response.content")));
					pLogic->SetStyle(true, false);
				}
			}
		}
	}
}

void UTrainGameInstance::NotifyUpdateUserExp()
{

}

void UTrainGameInstance::NotifyRespondUserCommand(const FString& cmd, bool exist)
{

}

void UTrainGameInstance::NotifyUserRankDataUploaded()
{

}
////////interface of trainonline end////////////



void UTrainGameInstance::SetTrainMode(ETrainMode tm)
{
	TrainMode = tm;
}

ETrainMode UTrainGameInstance::GetTrainMode() const
{
	return TrainMode;
}

void UTrainGameInstance::SetTaskPool( ATaskPool * task_pool)
{
	TaskPool = task_pool;
}

void UTrainGameInstance::SetCallbackActor(ACallbackActor * callback_actor)
{
	CallbackActor = callback_actor;
}

bool UTrainGameInstance::IsGameReady() const
{
	return GameReady;
}

void UTrainGameInstance::Init()
{
	UGameInstance::Init();
	TrainMode = TrainMode_Single_Player;
	Instance = this;
	TaskPool = nullptr;
	CallbackActor = nullptr;
	FString tmmt = ReadConfig(TEXT("Network.SocketServer"),TEXT("Network.TryMatchMaxTime"));
	if (!tmmt.IsEmpty() && tmmt.IsNumeric())
		TryMatchMaxTime = FCString::Atoi(*tmmt);
	else
		WriteConfig(TEXT("Network.SocketServer"), TEXT("Network.TryMatchMaxTime"), FString::FormatAsNumber(TryMatchMaxTime));

	if (0 == MEMBER_STAGE.Num())
	{
		MEMBER_STAGE.Add(TEXT("nr"), MEMBER_NOT_READY);
		MEMBER_STAGE.Add(TEXT("r"), MEMBER_READY);
		MEMBER_STAGE.Add(TEXT("gc"), MEMBER_CONNECTED);
		MEMBER_STAGE.Add(TEXT("gd"), MEMBER_DISCONNECTED);
		MEMBER_STAGE_REVERSE.Empty();
		for (auto& m : MEMBER_STAGE)
			MEMBER_STAGE_REVERSE.Add(m.Value,m.Key);
	}
	if (0 == LSM.Num())
	{
		LSM.Add(TEXT("state_create_requesting"), LOBBY_CREATE_REQUESTING);
		LSM.Add(TEXT("state_hosting"), LOBBY_HOSTING);
		LSM.Add(TEXT("state_launching"), LOBBY_LAUNCHING);
		LSM.Add(TEXT("state_role_selecting"), LOBBY_ROLE_SELECTING);
		LSM.Add(TEXT("state_destroy"), LOBBY_DESTROY);
		LSM.Add(TEXT("state_join_requesting"), LOBBY_JOIN_REQUESTING);
		LSM.Add(TEXT("state_host_changing"), LOBBY_HOST_CHANGING);
		LSM.Add(TEXT("state_locking"), LOBBY_LOCKING);
		LSM.Add(TEXT("state_leave"), LOBBY_LEAVE);
		LSM.Add(TEXT("state_in_game"), LOBBY_IN_GAME);
	}
	GameReady = true;

	UGlobalValueContainer::AssignSystemKeyValues();
}

void UTrainGameInstance::Shutdown()
{
	UGameInstance::Shutdown();
}

FString UTrainGameInstance::ReadConfig(const FString & section, const FString & field)
{
	FString val;
	if (nullptr == GConfig)
		return val;
	GConfig->GetString(*section, *field, val, GGameIni);
	return val;
}

bool UTrainGameInstance::WriteConfig(const FString & section, const FString & field, const FString & value)
{
	if (nullptr == GConfig)
		return false;
	GConfig->SetString(*section, *field, *value, GGameIni);
	GConfig->Flush(false, GGameIni);
	//GEngine->AddOnScreenDebugMessage(
	//	-1,
	//	20.f,
	//	FColor::Red,
	//	FString::Printf(TEXT("UTrainGameInstance::WriteConfig ~>%s"),*GGameUserSettingsIni));
	return true;
}

bool UTrainGameInstance::ServerInit(const FString & addr)
{
	TSharedPtr<FUserProducer>&& up = GetUserProducer();
	if (up.Get())
	{
		SetupHttpFunction(this);
		up->Init(addr,this);
		return true;
	}
	return false;	
}

bool UTrainGameInstance::UserLogin(const FString & id, const FString & psw)
{
	TSharedPtr<FUserProducer>&& up = GetUserProducer();
	if (up.Get())
		return up->PendingLogin(id, psw); 
	return false;
}

bool UTrainGameInstance::UserRegister()
{
	TSharedPtr<FUserProducer>&& up = GetUserProducer();
	if (up.Get())
		return up->PendingRegisterCurrentUser();
	return false;
}

bool UTrainGameInstance::UserAcceptDetailInfo(const FString& id)
{
	TSharedPtr<FUserProducer>&& up = GetUserProducer();
	if (up.Get())
		return up->PendingAcceptDetailInfo(id);
	return false;
}

bool UTrainGameInstance::UserExistVerify(const FString & id)
{
	TSharedPtr<FUserProducer>&& up = GetUserProducer();
	if (up.Get())
		return up->PendingVerifyUserExist(id);
	return false;
}

bool UTrainGameInstance::Handshake()
{
	TSharedPtr<FUserProducer>&& up = GetUserProducer();
	if (up.Get())
		return up->PendingHandshake();
	return false;
}

bool UTrainGameInstance::SubmitCurrentActionExp(int add_exp, int add_exp_duration)
{
	TSharedPtr<FUserProducer>&& up = GetUserProducer();
	if (up.Get())
		return up->PendingSumbitCurrentActionExp(add_exp, add_exp_duration);
	return false;
}

bool UTrainGameInstance::UserHttpCommandRecieve()
{
	TSharedPtr<FUserProducer>&& up = GetUserProducer();
	if (up.Get())
		return up->PendingRecieveUserCmd();
	return false;
}

void UTrainGameInstance::ClearCookies()
{
	IWebBrowserSingleton* iwbs = IWebBrowserModule::Get().GetSingleton();
	if (nullptr == iwbs)
		return;
	TSharedPtr<IWebBrowserCookieManager> cookieMan = iwbs->GetCookieManager();
	cookieMan->DeleteCookies();
}

bool UTrainGameInstance::VersionVerify()
{
	//verify apk version.
	TSharedPtr<FUserProducer>&& up = GetUserProducer();
	const SUser* u = up->GetClientUser().Get();
	if (!FUserProducer::InnerVersion.Equals(u->version_))
	{
		FString download_tips(u->download_tips_.IsEmpty()?
			TEXT("New version is available.Will you download it?"):
			u->download_tips_);
		FString download_caption(u->download_caption_.IsEmpty()?
			TEXT("Update"):
			u->download_caption_);
		FString no_wifi_tips(u->no_wifi_tips_.IsEmpty()?
			TEXT("No wifi detected, will you still want to download?"):
			u->no_wifi_tips_);
		FString apk_url_full = up->GetHttpWorker()->HttpServerAddress + u->apk_url_;
		EAppReturnType::Type Result = FPlatformMisc::MessageBoxExt(EAppMsgType::YesNo, *download_tips, *download_caption);
		if (EAppReturnType::Yes == Result)
		{
#if PLATFORM_ANDROID
			FJavaDataAccepter::UpdateAPK(apk_url_full, no_wifi_tips);
#endif
			return true;//need to update.
		}
	}
	//connect to the skynet server
	//start to connect socket server
	if (FSocketEnvWorker::SocketEnvWorker.Get())
	{
		FSocketEnvWorker::SocketEnvWorker->EnsureCompletion();
		FSocketEnvWorker::SocketEnvWorker.Reset();
	}
	FSocketEnvWorker::SocketEnvWorker = MakeShareable<FSocketEnvWorker>(new FSocketEnvWorker(GetUserProducer().Get(), GetSocketProducer().Get()));
	return false;//don't need to update.
}

bool UTrainGameInstance::IsOfflineMode()
{
	SUser* u = GetUserProducer()->GetClientUser().Get();
	if ( nullptr == u)
		return true;
	return u->id_.Equals(TEXT("su"),ESearchCase::IgnoreCase);
}

bool UTrainGameInstance::SetOfflineMode()
{
	SUser* u = GetUserProducer()->GetClientUser().Get();
	if (nullptr == u)
		return false;
	u->id_ = TEXT("su");
	u->name_ = TEXT("Offline");
	u->exp_ = 0.f;
	return true;
}

bool UTrainGameInstance::RequestCurrentLobbyResult()
{
	TSharedPtr<FUserProducer>&& up = GetUserProducer();
	SLobby* lobby = GetLobbyProducer()->GetCurrentLobby();
	if (up.Get() && lobby )
	{
		lobby->PendingToRank();
		for (auto member : lobby->members_)
			up->PendingReadUserRankResult(member.user_id_);
		return true;
	}
	return false;
}

FString UTrainGameInstance::GetCurrentLobbyResult()
{
	SLobby* lobby = GetLobbyProducer()->GetCurrentLobby();
	if (lobby)
		return lobby->RankJsonSerialize();
	return FString();
}

bool UTrainGameInstance::VisitUrlWithGetMethod(const FString& url, const FString& method)
{
	TSharedPtr<FUserProducer>&& up = GetUserProducer();
	if (up.Get())
		return up->PendingVisitUrl(url,method);
	return false;
}

bool UTrainGameInstance::ConnectSocketServer(const FString & socket_name, const FString server_ip, int32 port)
{
	//manually connected
	STrainSocket* ts = GetSocketProducer()->CreateSocket(socket_name, false); //using tcp
	if (nullptr == ts)
		return false;
	return GetSocketProducer()->ConnectToServer(ts, server_ip, port);
}

bool UTrainGameInstance::SendDataToSocketServer(const FString & socket_name, const FString & content,const FString proto_request,bool using_sproto)
{
	STrainSocket* ts = GetSocketProducer()->FindSocket(socket_name);
	if (nullptr == ts)
		return false;
	if( !using_sproto )
		return GetSocketProducer()->SendDataToServer(ts, content);
	else
	{
		//using sproto.
		//using binary sendformat
		//TSharedPtr<FSprotoPacker>&& lm = GetSprotoPacker();
		//if (lm.Get())
		//{
		//	FSprotoPacker::FSenderBufferPtr&& buf = lm->NetSprotoPack(proto_request, content);
		//	if (buf.Get() && ts->socket_data_sender_.Get() )
		//	{
		//		ts->socket_data_sender_->PushBuffer(buf->buf,buf->len);//value will be passed to it.
		//		buf->buf = 0;//memory should not be delete by it.
		//		buf->len = 0;
		//	}
		//	return true;
		//}
		if (ts->socket_data_sender_.Get())
		{
			ts->socket_data_sender_->PushBuffer(content, proto_request);//all will done in the send thread.
			return true;
		}			
	}
	return false;
}

bool UTrainGameInstance::SetupSocketReciever(const FString & socket_name)
{
	STrainSocket* ts = GetSocketProducer()->FindSocket(socket_name);
	if (nullptr == ts)
		return false;
	return GetSocketProducer()->SetupSocketReciever(ts, WrapperOfSocketPackageReciever);
}

//fill data with sproto lobby
void FillLobby(SLobby* L,sproto::lobby* l, UTrainGameInstance* ins)
{
	if (nullptr == L || nullptr == l || nullptr  == ins)
		return;
	static TMap<FString, FString> _user_name_id_map;
	//backup user-names.
	for (auto& u:L->members_)
	{
		if (!u.user_name_.IsEmpty())
			_user_name_id_map.FindOrAdd(u.user_id_) = u.user_name_;
	}
	L->Clear();
	L->name_ = UTF8_TO_TCHAR(l->lobby_name_);
	L->stage_ = (ELobbyStage)l->stage_;
	for (unsigned int i = 0; i < l->member_num_; ++i)
	{
		sproto::lobbymember& _lm = l->members_[i];
		SLobbyMember lm;
		lm.user_id_ = _lm.user_id_;
		lm.ability_ = _lm.ability_;
		lm.is_host_ = _lm.is_host_;
		lm.exp_gain_ = _lm.exp_gain_;
		lm.time_spend_ = _lm.time_spend_;

		lm.stage_ = MEMBER_STAGE[UTF8_TO_TCHAR(_lm.user_stage_)];
		lm.user_name_ = _user_name_id_map.FindOrAdd(lm.user_id_);
		if (lm.user_name_.IsEmpty())
			ins->UserAcceptDetailInfo(lm.user_id_);
		check(ins->GetTaskPool());
		for (unsigned int j = 0; j < _lm.task_action_num_; ++j)
		{
			sproto::lobbytaskaction& _la = _lm.task_actions_[j];
			SLobbyTaskAction la;
			la.action_id_ = _la.action_id_;
			la.item_id_ = _la.item_id_;
			la.task_id_ = _la.task_id_;
			la.action_status_ = _la.action_status_;
			//action status should not be rollback.
			STATIC_TASK_BASE::TaskItemActionPtr tap = ins->GetTaskPool()->GetSpecificTaskItemAction(
				la.task_id_,
				la.item_id_,
				la.action_id_);
			if (tap.Get() && tap->state > (EBaseState)_la.action_status_ )
				la.action_status_ = tap->state;
			lm.task_actions_.Add(la);
		}
		L->members_.Add(lm);
	}
	L->ReCalcuAverAbility();
}

void UTrainGameInstance::HandleSprotoDataFromQueue()
{
	if (nullptr == UTrainGameInstance::Instance
		|| (UTrainGameInstance::Instance
		&& ETrainMode::TrainMode_Single_Player == UTrainGameInstance::Instance->GetTrainMode()))
	{
		return;
		//don't need to process in the single-player mode.
	}
	STrainSocket* ts = GetSocketProducer()->FindSocket(FSocketProducer::DefaultSocketServerName);
	SUser* u = GetUserProducer()->GetClientUser().Get();
	TSharedPtr<FSprotoPacker>&& lm = GetSprotoPacker();
	if (nullptr == ts || nullptr == u || nullptr == lm.Get() )
		return;
	SLobby* lobby = GetLobbyProducer()->GetCurrentLobby();
	TArray<FSprotoPacker::FSprotoBufferPtr>&& buffers = lm->GetRecieverData();
	for(auto& d:buffers)
	{
		//any of the message.
		FSocketDataReciever* sdr = ts->socket_data_reciever_.Get();
		if (sdr)
		{
			if (!sdr->IsReadyToBeat())
			{
				sdr->StartHeartBeat();
				sdr->HeartBeat();//at very begin.
			}
			sdr->HeartBeat();
		}
		switch (d->type_)
		{
		case sproto::sproto_type::HANDSHAKE:
			{
				sproto::sproto_handshake* r = static_cast<sproto::sproto_handshake*>(d.Get());
				if (r->in_lobby_)
				{
					if (nullptr == lobby)
					{
						//out of term. told the server need to leave.
						FString to_server = FString::Printf(TEXT("{lobbyname='',user_id='%s'}"),
							*GetUserProducer()->GetClientUser()->id_);
						Instance->SendDataToSocketServer(FSocketProducer::DefaultSocketServerName,
							to_server, "leavelobby");
					}
				}
				GEngine->AddOnScreenDebugMessage(
					-1,
					20.f,
					FColor::Green,
					FString::Printf(TEXT("UTrainGameInstance::HandleSprotoDataFromQueue[HANDSHAKE]~>[Agent.index]%d,%s,in-lobby:%s"), r->agent_index_, UTF8_TO_TCHAR(r->msg_),r->in_lobby_?TEXT("true"):TEXT("false")));
			}
			break;
		case sproto::sproto_type::HEARTBEAT:
			{
				sproto::sproto_heartbeat * r = static_cast<sproto::sproto_heartbeat*>(d.Get());
				//GEngine->AddOnScreenDebugMessage(
				//	-1,
				//	20.f,
				//	FColor::Green,
				//	FString::Printf(TEXT("UTrainGameInstance::HandleSprotoDataFromQueue[HEARTBEAT]")));
			}
			break;
		case sproto::sproto_type::GET:
			{
				sproto::sproto_get* r = static_cast<sproto::sproto_get*>(d.Get());
				GEngine->AddOnScreenDebugMessage(
					-1,
					20.f,
					FColor::Green,
					FString::Printf(TEXT("UTrainGameInstance::HandleSprotoDataFromQueue[GET]~>%s"), UTF8_TO_TCHAR(r->value_)));
			}
			break;
		case sproto::sproto_type::MOVEMENT:
			{
				sproto::sproto_movement* r = static_cast<sproto::sproto_movement*>(d.Get());
				for (unsigned int i = 0; i < r->sm_len_; ++i)
				{
					const sproto::movement& sm = r->sm_[i];
					FTransform tf;
					tf.SetLocation(FVector(sm.px_, sm.py_, sm.pz_));
					tf.SetRotation(FQuat(FRotator(sm.ep_, sm.ey_, sm.er_)));
					tf.SetScale3D(FVector(sm.sx_, sm.sy_, sm.sz_));
					AHumanCharacter::FSynchronizeSender::SyncFromServer(
						UTF8_TO_TCHAR(r->role_),
						UTF8_TO_TCHAR(sm.n_),
						tf,
						FVector(sm.vx_, sm.vy_, sm.vz_)
						);
					UE_LOG(RaiiGameInstance, Log, TEXT("UTrainGameInstance::HandleSprotoDataFromQueue[MOVEMENT]~>role:%s,animation:%s,data[%d]:location~>%.2f,%.2f,%.2f; rotation~>%.2f,%.2f,%.2f; scale~>%.2f,%.2f,%.2f; velocity~>%.2f,%.2f,%.2f;"),
							UTF8_TO_TCHAR(r->role_),
							UTF8_TO_TCHAR(sm.n_),
							i + 1,
							sm.px_, sm.py_, sm.pz_,
							sm.ey_, sm.ep_, sm.er_,
							sm.sx_, sm.sy_, sm.sz_,
							sm.vx_, sm.vy_, sm.vz_);
					//GEngine->AddOnScreenDebugMessage(
					//	-1,
					//	20.f,
					//	FColor::Green,
					//	FString::Printf(TEXT("UTrainGameInstance::HandleSprotoDataFromQueue[MOVEMENT]~>role:%s,animation:%s,data[%d]:location~>%.2f,%.2f,%.2f; \nrotation~>%.2f,%.2f,%.2f; scale~>%.2f,%.2f,%.2f; velocity~>%.2f,%.2f,%.2f;"),
					//		UTF8_TO_TCHAR(r->role_),
					//		UTF8_TO_TCHAR(sm.n_),
					//		i+1,
					//		sm.px_, sm.py_, sm.pz_,
					//		sm.ey_, sm.ep_, sm.er_,
					//		sm.sx_, sm.sy_, sm.sz_,
					//		sm.vx_, sm.vy_, sm.vz_
					//		));
				}
			}
			break;
		case sproto::sproto_type::MESSAGE:
			{
				sproto::sproto_message* r = static_cast<sproto::sproto_message*>(d.Get());
				if (r->what_)
				{
					FString msg(UTF8_TO_TCHAR(r->what_));
					if (0 == msg.Find(LobbyFinishMark))
					{
						TPair<bool, TArray<FString>>* upr = Instance->UserPlayRoles.Find(msg.Mid(LobbyFinishMark.Len()));
						if (nullptr != upr)
							upr->Key = true;
						bool all_finish_select = true;
						for (auto& _upr : Instance->UserPlayRoles)
						{
							all_finish_select &= _upr.Value.Key;
						}
						if (Instance->CallbackActor)
						{
							Instance->CallbackActor->SetCallbackType(Callback_NotifyLobbyRoleSelectFinished);
							Instance->CallbackActor->NotifyLobbyRoleSelectFinished(all_finish_select);
						}
					}
					else
					{
						//world|uid|uname|hello
						//team|uid|uname|hello
						//taskcomplete-happy-ending-please
						if (msg.Mid(0, 6).Equals(TEXT("world|")))
						{
							//as world
							FString lhs;
							FString rhs;
							msg = msg.Mid(6);
							msg.Split(TEXT("|"), &lhs, &rhs);
							FString uid = lhs;
							rhs.Split(TEXT("|"), &lhs, &rhs);
							Instance->CallbackActor->SetCallbackType(Callback_NotifyMessageRecieve);
							Instance->CallbackActor->NotifyMessageRecieve(rhs, uid, lhs, E_MSG_WORLD);
						}
						else if (msg.Mid(0, 5).Equals(TEXT("team|")))
						{
							//as team
							FString lhs;
							FString rhs;
							msg = msg.Mid(5);
							msg.Split(TEXT("|"), &lhs, &rhs);
							FString uid = lhs;
							rhs.Split(TEXT("|"), &lhs, &rhs);
							Instance->CallbackActor->SetCallbackType(Callback_NotifyMessageRecieve);
							Instance->CallbackActor->NotifyMessageRecieve(rhs, uid, lhs, E_MSG_TEAM);
						}
						else if (msg.Equals(TEXT("taskcomplete-happy-ending-please")))
						{
							//notify a happy ending.
							Instance->CallbackActor->SetCallbackType(Callback_NotifyHappyEnding);
							Instance->CallbackActor->NotifyHappyEnding();
						}
						else//OTHERS MAY BE IN PRIVATE.
						{
							Instance->AcceptPassbyRoleTaskActionStatus(msg);
						}
					}
				}
				UE_LOG(RaiiGameInstance, Log, TEXT("UTrainGameInstance::HandleSprotoDataFromQueue[MESSAGE]~>fromuser:%s,agentindex:%d,msg:%s"),
					UTF8_TO_TCHAR(r->user_id_), r->from_agent_index_, UTF8_TO_TCHAR(r->what_));
				//GEngine->AddOnScreenDebugMessage(
				//	-1,
				//	20.f,
				//	FColor::Green,
				//	FString::Printf(TEXT("UTrainGameInstance::HandleSprotoDataFromQueue[MESSAGE]~>fromuser:%s,agentindex:%d,msg:%s"), 
				//		UTF8_TO_TCHAR(r->user_id_),r->from_agent_index_, UTF8_TO_TCHAR(r->what_)));
			}
			break;
		case sproto::sproto_type::QUERY_UID:
			{
				sproto::sproto_queryuid* r = static_cast<sproto::sproto_queryuid*>(d.Get());
				GEngine->AddOnScreenDebugMessage(
					-1,
					20.f,
					FColor::Green, 
					FString::Printf(TEXT("UTrainGameInstance::HandleSprotoDataFromQueue[QUERY_UID]~>founduser:%s[%s]"),
						UTF8_TO_TCHAR(r->uid_),r->uid_found_?TEXT("found"):TEXT("not found") ));
			}
			break;
		case sproto::sproto_type::CREATE_LOBBY:
			{
				sproto::sproto_createlobby* r = static_cast<sproto::sproto_createlobby*>(d.Get());
				if (r->succeed_)
				{
					//notify surface panel.
					if (Instance->CallbackActor)
					{
						Instance->CallbackActor->SetCallbackType(Callback_NotifyLobbyUpdate);
						Instance->CallbackActor->NotifyLobbyUpdate();
					}
				}
				GEngine->AddOnScreenDebugMessage(
					-1,
					20.f,
					FColor::Green,
					FString::Printf(TEXT("UTrainGameInstance::HandleSprotoDataFromQueue[CREATE_LOBBY]~>succeed:%s"),
						r->succeed_ ? TEXT("true") : TEXT("false")));
			}
			break;
		case sproto::sproto_type::JOIN_LOBBY:
			{
				sproto::sproto_joinlobby* r = static_cast<sproto::sproto_joinlobby*>(d.Get());
				if (lobby && r->succeed_)
				{
					FillLobby(lobby, r->lobby_, Instance);
					//new average ability should be update to the server.
					int* a = (int*)&lobby->averability_;
					FString to_server = FString::Printf(TEXT("{lobbyname='%s',stage=%d,averability=%d}"),
						*lobby->name_, (int)lobby->stage_, *a);
					Instance->SendDataToSocketServer(FSocketProducer::DefaultSocketServerName,
						to_server, "updatelobby");
					GEngine->AddOnScreenDebugMessage(
						-1,
						20.f,
						FColor::Green,
						FString::Printf(TEXT("UTrainGameInstance::HandleSprotoDataFromQueue[JOIN_LOBBY]~>succeed:%s,lobby:%s,aver_ability:%f,member_count:%d"),
							r->succeed_ ? TEXT("true") : TEXT("false"),
							*lobby->name_,lobby->averability_,lobby->members_.Num()));
					UE_LOG(RaiiGameInstance, Log, TEXT("GameInstance::JoinMatch~>join-ok,after try %d times."), MatchJoinTries);
				}
				else
				{
					//try to re-join again.
					if (Instance->TryMatchMaxTime < MatchJoinTries)
					{
						//give a message that there's no lobbies.
						UE_LOG(RaiiGameInstance, Log, TEXT("GameInstance::JoinMatch~>try max time at %d, terminated."), MatchJoinTries);
						if (Instance->CallbackActor)
						{
							Instance->CallbackActor->SetCallbackType(Callback_NotifyMatchLobbyTryMaxTime);
							Instance->CallbackActor->NotifyMatchLobbyTryMaxTime();
							MatchJoinTries = 0;
							break;
						}
					}
					MatchJoinTries++;
					UE_LOG(RaiiGameInstance, Log, TEXT("GameInstance::JoinMatch~>send try %d"), MatchJoinTries);
					if (lobby && lobby->LobbyStateMachine.Get())
					{
						lobby->LobbyStateMachine->SetNextStateFunction("state_join_requesting");
						lobby->LobbyStateMachine->ExplainCurrentFunction();
					}
				}
			}
			break;
		case sproto::sproto_type::UPDATE_LOBBY:
			{
				sproto::sproto_updatelobby* r = static_cast<sproto::sproto_updatelobby*>(d.Get());
				if (lobby && r->succeed_)
				{
					//notify surface panel
					if (Instance->CallbackActor)
					{
						if (LOBBY_LAUNCHING >= lobby->stage_)
						{
							Instance->CallbackActor->SetCallbackType(Callback_NotifyLobbyUpdate);
							Instance->CallbackActor->NotifyLobbyUpdate();
						}
						else if (LOBBY_LOCKING == lobby->stage_)
						{
							Instance->CallbackActor->SetCallbackType(Callback_NotifyLobbyReadyAsked);
							Instance->CallbackActor->NotifyLobbyReadyAsked();
						}
					}
				}
				//GEngine->AddOnScreenDebugMessage(
				//	-1,
				//	20.f,
				//	FColor::Green,
				//	FString::Printf(TEXT("UTrainGameInstance::HandleSprotoDataFromQueue[UPDATE_LOBBY]~>succeed:%s"),
				//		r->succeed_ ? TEXT("true") : TEXT("false")));
			}
			break;
		case sproto::sproto_type::LOBBY_CHANGED:
			{
				if (lobby)
				{
					sproto::sproto_lobby_changed* r = static_cast<sproto::sproto_lobby_changed*>(d.Get());
					FillLobby(lobby, r->lobby_, Instance);
					if (LOBBY_IN_GAME != lobby->stage_)
					{
						Instance->AcceptUserRoleSelectChange();
						//notify surface panel.
						if (Instance->CallbackActor)
						{
							Instance->CallbackActor->SetCallbackType(Callback_NotifyLobbyUpdate);
							Instance->CallbackActor->NotifyLobbyUpdate();
						}
					}
					else
						Instance->AcceptActionStatusChange();
					UE_LOG(RaiiGameInstance, Log, TEXT("UTrainGameInstance::HandleSprotoDataFromQueue[LOBBY_CHANGED]~>lobby:%s,aver_ability:%f,member_count:%d"),
						*lobby->name_, lobby->averability_, lobby->members_.Num());
					//GEngine->AddOnScreenDebugMessage(
					//	-1,
					//	20.f,
					//	FColor::Green,
					//	FString::Printf(TEXT("UTrainGameInstance::HandleSprotoDataFromQueue[LOBBY_CHANGED]~>lobby:%s,aver_ability:%f,member_count:%d"),
					//		*lobby->name_, lobby->averability_, lobby->members_.Num()));
				}
			}
			break;
		case sproto::sproto_type::HOST_CHANGED:
			{
				if (lobby)
				{
					sproto::sproto_host_changed* r = static_cast<sproto::sproto_host_changed*>(d.Get());
					SLobbyMember* member = lobby->FindMember(GetUserProducer()->GetClientUser()->id_);
					if (member)
						member->is_host_ = true;
					GEngine->AddOnScreenDebugMessage(
						-1,
						20.f,
						FColor::Green,
						FString::Printf(TEXT("UTrainGameInstance::HandleSprotoDataFromQueue[HOST_CHANGED]~>set as host")));

				}
			}
			break;
		case sproto::sproto_type::GAME_READY:
			{
				if (lobby)
				{
					sproto::sproto_game_ready* r = static_cast<sproto::sproto_game_ready*>(d.Get());
					FillLobby(lobby, r->lobby_, Instance);
					Instance->AcceptUserRoleSelectChange();
					//notify surface panel.
					if (Instance->CallbackActor)
					{
						Instance->CallbackActor->SetCallbackType(Callback_NotifyLobbyUpdate);
						Instance->CallbackActor->NotifyLobbyUpdate();
						Instance->CallbackActor->SetCallbackType(Callback_NotifyLobbyReadyAsked);
						Instance->CallbackActor->NotifyLobbyReadyAsked();
					}
					GEngine->AddOnScreenDebugMessage(
						-1,
						20.f,
						FColor::Green,
						FString::Printf(TEXT("UTrainGameInstance::HandleSprotoDataFromQueue[GAME_READY]~>stage:%d,need to ask the client whether is ready or not.")
							, r->lobby_->stage_));
				}
			}
			break;
		case sproto::sproto_type::GAME_LAUNCH:
			{
				if (lobby)
				{
					sproto::sproto_game_ready* r = static_cast<sproto::sproto_game_ready*>(d.Get());
					FillLobby(lobby, r->lobby_, Instance);
					Instance->AcceptUserRoleSelectChange();
					//notify surface panel.
					if (Instance->CallbackActor)
					{
						Instance->CallbackActor->SetCallbackType(Callback_NotifyLobbyLaunchedOn);
						Instance->CallbackActor->NotifyLobbyLaunchedOn();
					}
					//empty user-role map
					Instance->UserPlayRoles.Empty();
					GEngine->AddOnScreenDebugMessage(
						-1,
						20.f,
						FColor::Green,
						FString::Printf(TEXT("UTrainGameInstance::HandleSprotoDataFromQueue[GAME_LAUNCH]~>launch the game right now!")));
				}
			}
			break;
		case sproto::sproto_type::ASSIGN_MEMBER_ACTION:
			{
				sproto::sproto_assignmemberaction* r = static_cast<sproto::sproto_assignmemberaction*>(d.Get());
				if (r->succeed_)
				{
					//notify surface panel.
					if (Instance->CallbackActor)
					{
						Instance->CallbackActor->SetCallbackType(Callback_NotifyLobbyUpdate);
						Instance->CallbackActor->NotifyLobbyUpdate();
					}
				}
				UE_LOG(RaiiGameInstance, Log, TEXT("UTrainGameInstance::HandleSprotoDataFromQueue[ASSIGN_MEMBER_ACTION]~>succeed:%s"),
					r->succeed_ ? TEXT("true") : TEXT("false"));
				//GEngine->AddOnScreenDebugMessage(
				//	-1,
				//	20.f,
				//	FColor::Green,
				//	FString::Printf(TEXT("UTrainGameInstance::HandleSprotoDataFromQueue[ASSIGN_MEMBER_ACTION]~>succeed:%s"),
				//		r->succeed_ ? TEXT("true") : TEXT("false")));
			}
			break;
		case sproto::sproto_type::UPDATE_MEMBER_ACTION:
			{
				sproto::sproto_updatememberaction* r = static_cast<sproto::sproto_updatememberaction*>(d.Get());
				if (r->succeed_)
				{
				}
				UE_LOG(RaiiGameInstance, Log, TEXT("UTrainGameInstance::HandleSprotoDataFromQueue[UPDATE_MEMBER_ACTION]~>succeed:%s"),
					r->succeed_ ? TEXT("true") : TEXT("false"));
				//GEngine->AddOnScreenDebugMessage(
				//	-1,
				//	20.f,
				//	FColor::Green,
				//	FString::Printf(TEXT("UTrainGameInstance::HandleSprotoDataFromQueue[UPDATE_MEMBER_ACTION]~>succeed:%s"),
				//		r->succeed_ ? TEXT("true") : TEXT("false")));
			}
			break;
		}
	};
}

FString UTrainGameInstance::BuildMovementString(const FVector & location, const FRotator & rotation, const FVector& scale, const FVector& velocity, const FString & animation)
{
	int32 l[3], r[3], s[3], v[3];
	l[0] = *(int32*)&location.X;
	l[1] = *(int32*)&location.Y;
	l[2] = *(int32*)&location.Z;

	r[0] = *(int32*)&rotation.Roll;
	r[1] = *(int32*)&rotation.Pitch;
	r[2] = *(int32*)&rotation.Yaw;

	s[0] = *(int32*)&scale.X;
	s[1] = *(int32*)&scale.Y;
	s[2] = *(int32*)&scale.Z;

	v[0] = *(int32*)&velocity.X;
	v[1] = *(int32*)&velocity.Y;
	v[2] = *(int32*)&velocity.Z;

	return FString::Printf(TEXT("{location={%d,%d,%d},rotation={%d,%d,%d},scale={%d,%d,%d},velocity={%d,%d,%d},animation='%s'}"),
		l[0],l[1],l[2],r[0],r[1],r[2],s[0],s[1],s[2], v[0],v[1],v[2],*animation);
}

FString UTrainGameInstance::BuildMovementSendString(const FString& move_actor_name, const TArray<FString>& data, TArray<FString>* towhom)
{
	TSharedPtr<FUserProducer>&& up = GetUserProducer();
	const SUser* u = up->GetClientUser().Get();
	FString data_ = TEXT("data = {");
	for (auto& di : data)
		data_.Append(FString::Printf(TEXT("%s,"), *di));
	data_.RemoveFromEnd(TEXT(","));
	data_.Append(TEXT("}"));
	FString role_ = FString::Printf(TEXT("role = '%s'"), *move_actor_name);
	FString towhom_ = TEXT("who = {");

	if (nullptr != towhom)
	{
		//using the reiever speicifced outside.
		for (auto& wi : *towhom)
			towhom_.Append(FString::Printf(TEXT("%s,"), *wi));
	}
	else
	{
		//using current lobby. members,except self.
		TSharedPtr<FLobbyProducer>&& lp = GetLobbyProducer();
		SLobby* lb = lp->GetCurrentLobby();
		if (nullptr == lb)
			return FString();
		int to_whom_count = 0;
		for (auto& m : lb->members_)
		{
			if (!u->id_.Equals(m.user_id_, ESearchCase::IgnoreCase))
			{
				towhom_.Append(FString::Printf(TEXT("'%s',"), *m.user_id_));
				to_whom_count++;
			}
		}
		if (0 == to_whom_count)
			return FString();//empty
	}

	towhom_.RemoveFromEnd(TEXT(","));
	towhom_.Append(TEXT("}"));
	return FString::Printf(TEXT("{%s,%s,%s}"), *data_, *towhom_, *role_);
}

ATaskPool * UTrainGameInstance::GetTaskPool()
{
	return TaskPool;
}

FString UTrainGameInstance::BuildHandshakeString()
{
	TSharedPtr<FUserProducer>&& up = GetUserProducer();
	const SUser* u = up->GetClientUser().Get();
	return FString::Printf(TEXT("{userid='%s'}"), u->id_.IsEmpty() ? TEXT("all4qianqian") : *u->id_);
}

bool UTrainGameInstance::CreateLobby(const FString & name)
{
	TSharedPtr<FLobbyProducer>&& lp = GetLobbyProducer();
	SLobby* lb = lp->CreateLobby(name);
	if (nullptr != lb)
	{
		//setup lobby state machine.
		lb->LobbyStateMachine->SetupFSM(lb, LobbyStateCallFunc);
		lb->LobbyStateMachine->ExplainCurrentFunction();//explain!
		return true;
	}
	return false;
}

bool UTrainGameInstance::LeaveCurrentLobby()
{
	TSharedPtr<FLobbyProducer>&& lp = GetLobbyProducer();
	SLobby* lb = lp->GetCurrentLobby();
	if (lb)
	{
		//jump to the leaving state function
		if( !lb->LobbyStateMachine->IsFSMSetuped() )
			lb->LobbyStateMachine->SetupFSM(lb, LobbyStateCallFunc);
		lb->LobbyStateMachine->SetNextStateFunction("state_leave");
		lb->LobbyStateMachine->ExplainCurrentFunction();
		return lp->LeaveCurrentLobby();
	}
	return false;
}

bool UTrainGameInstance::MatchLobby()
{
	TSharedPtr<FLobbyProducer>&& lp = GetLobbyProducer();
	SLobby* lb = lp->JoinLobby();
	if (lb)
	{
		lb->stage_ = LOBBY_JOIN_REQUESTING;
		if (!lb->LobbyStateMachine->IsFSMSetuped())
			lb->LobbyStateMachine->SetupFSM(lb, LobbyStateCallFunc);
		lb->LobbyStateMachine->SetNextStateFunction("state_join_requesting");
		lb->LobbyStateMachine->ExplainCurrentFunction();
		return true;
	}
	return false;
}

bool UTrainGameInstance::LockLobby(bool lock)
{
	TSharedPtr<FLobbyProducer>&& lp = GetLobbyProducer();
	SLobby* lb = lp->GetCurrentLobby();
	if (lb)
	{
		//jump to the lock state function
		if (!lb->LobbyStateMachine->IsFSMSetuped())
			lb->LobbyStateMachine->SetupFSM(lb, LobbyStateCallFunc);
		if (lock)
		{
			lb->stage_ = LOBBY_LOCKING;
			lb->LobbyStateMachine->SetNextStateFunction("state_locking");
			lb->LobbyStateMachine->ExplainCurrentFunction();
		}
		else
		{
			lb->stage_ = LOBBY_HOSTING;
			lb->LobbyStateMachine->SetNextStateFunction("state_hosting");
			lb->LobbyStateMachine->ExplainCurrentFunction();
		}
		return true;
	}
	return false;
}

bool UTrainGameInstance::ReadyForPlay(bool ready )
{
	//if the user is not ready,just try to unlock the lobby so others may join in.
	TSharedPtr<FLobbyProducer>&& lp = GetLobbyProducer();
	SLobby* lb = lp->GetCurrentLobby();
	if (lb)
	{
		//jump to the lock state function
		if (!lb->LobbyStateMachine->IsFSMSetuped())
			lb->LobbyStateMachine->SetupFSM(lb, LobbyStateCallFunc);
		if (ready)
		{
			lb->LobbyStateMachine->SetNextStateFunction("state_launching");
			lb->LobbyStateMachine->ExplainCurrentFunction();
			return true;
		}
		else
		{
			lb->LobbyStateMachine->SetNextStateFunction("state_leave");
			lb->LobbyStateMachine->ExplainCurrentFunction();//leave first.

			lb->LobbyStateMachine->SetNextStateFunction("state_hosting");//changing to host
			lb->LobbyStateMachine->ExplainCurrentFunction();

			lp->LeaveCurrentLobby();//leave now!
		}
	}
	return false;
}

void UTrainGameInstance::AssignLobbyUserActions()
{
	TSharedPtr<FLobbyProducer>&& lp = GetLobbyProducer();
	SLobby* lb = lp->GetCurrentLobby();
	FString uid = GetUserProducer()->GetClientUser()->id_;
	if (lb)
	{
		//build the request string for the server.
		SLobbyMember* member = lb->FindMember(uid);
		if (member)
		{
			FString to_server = FString::Printf(TEXT("{lobbyname = '%s',user_id='%s',actions=%s}"),
				*lb->name_, *uid, *member->ActionSerialize());
			Instance->SendDataToSocketServer(FSocketProducer::DefaultSocketServerName,
				to_server, "assignmemberaction");
		}
	}
}

void UTrainGameInstance::UpdateLobbyUserAction(int nid, int tid, int aid, int status)
{
	TSharedPtr<FLobbyProducer>&& lp = GetLobbyProducer();
	SLobby* lb = lp->GetCurrentLobby();
	FString uid = GetUserProducer()->GetClientUser()->id_;
	if (lb)
	{
		//build the request string for the server.
		SLobbyMember* member = lb->FindMember(uid);
		if (member)
		{
			SLobbyTaskAction lta;
			lta.action_id_ = aid;
			lta.item_id_ = tid;
			lta.task_id_ = nid;
			int32 index = member->task_actions_.Find(lta);
			if (INDEX_NONE != index)
			{
				SLobbyTaskAction& _lta = member->task_actions_[index];
				_lta.action_status_ = status;
				FString to_server = FString::Printf(TEXT("{lobbyname = '%s',user_id='%s',action=%s}"),
					*lb->name_, *uid, *_lta.Serialize());
				Instance->SendDataToSocketServer(FSocketProducer::DefaultSocketServerName,
					to_server, "updatememberaction");
				//
				if (EBaseState::BState_FINISHED == status)
				{
					STATIC_TASK_BASE::TaskItemActionPtr tap = TaskPool->GetSpecificTaskItemAction(
						nid,
						tid,
						aid);
					member->exp_gain_ += tap->action_exp_benchmark_;
					member->time_spend_ += FMath::Max(0.f, tap->action_operation_end_time_ - tap->action_operation_start_time_);
					//update member's attribute
					to_server = FString::Printf(TEXT("{lobbyname = '%s',user_id='%s',attribute={{attr_name='exp_gain',attr_value='%d'},{attr_name='time_spend',attr_value='%d'}}}"),
						*lb->name_, *uid, member->exp_gain_, (int)member->time_spend_);
					Instance->SendDataToSocketServer(FSocketProducer::DefaultSocketServerName,
						to_server, "updatememberattribute");
				}
			}
		}
	}
}

void UTrainGameInstance::AddUserPlayerRole( const FString & uid, const FString & role_name)
{
	TPair<bool,TArray<FString>>& roles = UserPlayRoles.FindOrAdd(uid);
	roles.Value.AddUnique(role_name);
	//only local user need to be sent.
	//actually,it is not neccessary to use the finiti state machine.
	if (GetUserProducer()->GetClientUser()->id_.Equals(uid,ESearchCase::IgnoreCase))
	{
		TSharedPtr<FLobbyProducer>&& lp = GetLobbyProducer();
		SLobby* lb = lp->GetCurrentLobby();
		if (lb && TaskPool)
		{
			TArray<FTaskItemAction> actions;
			if (TaskPool->GetTaskItemActionByRoleName(role_name, actions))
			{
				SLobbyMember* member = lb->FindMember(uid);
				if (member)
				{
					for (auto& act : actions)
					{
						SLobbyTaskAction lta;
						lta.action_id_ = act.ActionIndex;
						lta.item_id_ = act.TaskItemIndex;
						lta.task_id_ = act.TaskNodeIndex;
						lta.action_status_ = act.ActState;
						member->task_actions_.AddUnique(lta);
					}
					AssignLobbyUserActions();
				}
			}
		}
	}
}

void UTrainGameInstance::RemoveUserPlayerRole(const FString & uid, const FString & role_name)
{
	TPair<bool,TArray<FString>>* roles = UserPlayRoles.Find(uid);
	if (roles)
	{
		int32 i = roles->Value.Find(role_name);
		if (INDEX_NONE != i)
		{
			roles->Value.RemoveAt(i);
			//only local user need to be sent.
			//actually,it is not neccessary to use the finiti state machine.
			if (GetUserProducer()->GetClientUser()->id_.Equals(uid))
			{
				TSharedPtr<FLobbyProducer>&& lp = GetLobbyProducer();
				SLobby* lb = lp->GetCurrentLobby();
				if (lb && TaskPool)
				{
					TArray<FTaskItemAction> actions;
					if (TaskPool->GetTaskItemActionByRoleName(role_name, actions))
					{
						SLobbyMember* member = lb->FindMember(uid);
						if (member)
						{
							for (auto& act : actions)
							{
								SLobbyTaskAction lta;
								lta.action_id_ = act.ActionIndex;
								lta.item_id_ = act.TaskItemIndex;
								lta.task_id_ = act.TaskNodeIndex;
								member->task_actions_.Remove(lta);
							}
							AssignLobbyUserActions();
						}
					}
				}
			}
		}	
	}
}

bool UTrainGameInstance::GetUserPlayerRoles(const FString & uid, FUserAsRoles& roles)
{
	roles.UserID = uid;
	roles.IsLocal = GetUserProducer()->GetClientUser()->id_.Equals(uid);
	roles.RoleNames.Empty();
	TPair<bool,TArray<FString>>* r = UserPlayRoles.Find(uid);
	if (r)
	{
		for (auto& _r : r->Value)
			roles.RoleNames.Add(_r);
		return true;
	}
	return false;
}

bool UTrainGameInstance::CheckUserRole(const FString & role_name, bool & is_local_role)
{
	is_local_role = false;
	for (auto& ur : UserPlayRoles)
	{
		if (INDEX_NONE != ur.Value.Value.Find(role_name))
		{
			is_local_role = GetUserProducer()->GetClientUser()->id_.Equals(ur.Key, ESearchCase::IgnoreCase);
			return true;
		}
	}
	return false;
}

bool UTrainGameInstance::CheckUserFinishRoleSelect(const FString & user_id)
{
	const TPair<bool, TArray<FString>>* r = UserPlayRoles.Find(user_id);
	if (nullptr != r)
		return r->Key;
	return false;
}

int UTrainGameInstance::CheckUserRoleAllSelect()
{
	if (nullptr == TaskPool)
		return -1;
	int role_count = TaskPool->GetCurrentRoleCount();
	int role_select = 0;
	for (auto& r : UserPlayRoles)
		role_select += r.Value.Value.Num();
	return role_count - role_select;
}

bool UTrainGameInstance::GetUserNameAsRole(const FString & role_name, FString & user_name)
{
	user_name.Empty();
	TSharedPtr<FLobbyProducer>&& lp = GetLobbyProducer();
	SLobby* lb = lp->GetCurrentLobby();
	if (nullptr == lb)
		return false;
	for (auto& ur : UserPlayRoles)
	{
		int32 index = ur.Value.Value.Find(role_name);
		if (INDEX_NONE != index )
		{
			SLobbyMember* member = lb->FindMember(ur.Key);
			if (nullptr != member)
			{
				user_name = member->user_name_;
				return true;
			}
		}
	}
	return false;
}

bool UTrainGameInstance::GetRoleNameAsUserID(const FString & user_id, FString & role_name)
{
	role_name.Empty();
	if (TaskPool
		&& GetUserProducer().Get()
		&& GetUserProducer()->GetClientUser().Get()
		&& GetUserProducer()->GetClientUser()->id_.Equals(user_id, ESearchCase::IgnoreCase))
	{
		//will return the current role name.
		role_name = TaskPool->GetCurrentRoleName();
		if (!role_name.IsEmpty())
			return true;
	}
	TSharedPtr<FLobbyProducer>&& lp = GetLobbyProducer();
	SLobby* lb = lp->GetCurrentLobby();
	if (nullptr == lb)
		return false;
	const TPair<bool,TArray<FString>>* r = UserPlayRoles.Find(user_id);
	if(nullptr != r && 0 < r->Value.Num() )
	{
		//first one.
		role_name = r->Value[0];
		return true;
	}
	return false;
}

bool UTrainGameInstance::GetUserIDAsRoleName(const FString & role_name, FString & user_id)
{
	user_id.Empty();
	TSharedPtr<FLobbyProducer>&& lp = GetLobbyProducer();
	SLobby* lb = lp->GetCurrentLobby();
	if (nullptr == lb)
		return false;
	for (auto& ur : UserPlayRoles)
	{
		int32 index = ur.Value.Value.Find(role_name);
		if (INDEX_NONE != index)
		{
			SLobbyMember* member = lb->FindMember(ur.Key);
			if (nullptr != member)
			{
				user_id = member->user_id_;
				return true;
			}
		}
	}
	return false;
}

int UTrainGameInstance::GetProperIndexOfRoleAsFirstRole(const FString & user_id, const TArray<int>& match_rule)
{
	TSharedPtr<FLobbyProducer>&& lp = GetLobbyProducer();
	SLobby* lb = lp->GetCurrentLobby();
	if (nullptr == lb || nullptr == TaskPool)
		return -1;
	const TPair<bool,TArray<FString>>* r = UserPlayRoles.Find(user_id);
	if (nullptr != r && 0 < r->Value.Num())
	{
		TArray<TPair<int,int>> to_be_sort;
		for (auto& name : r->Value)
		{
			int index = TaskPool->GetIndexOfRoleByRoleName(name);
			if (INDEX_NONE != index)
			{
				int _index = TaskPool->GetRoleIndex(index);
				if(INDEX_NONE != _index)
					to_be_sort.Add(TPairInitializer<int, int>(_index, match_rule.IndexOfByKey(_index)));//will sort by the role-index(not the index of role in the role-list.)
			}
		}
		if (to_be_sort.Num() > 0)
		{
			to_be_sort.Sort([](const TPair<int,int>& LHS, const TPair<int, int>& RHS) {
				return  LHS.Value < RHS.Value;//the match_rule must be contain all of the indexes.
			});
			//the first one is the proper one.
			return TaskPool->GetIndexOfRole(to_be_sort[0].Key);
		}
	}
	return -1;
}

bool UTrainGameInstance::DispatchMsgToLobbyMember(const FString & msg, bool as_host)
{
	TSharedPtr<FLobbyProducer>&& lp = GetLobbyProducer();
	SLobby* lb = lp->GetCurrentLobby();
	if (lb)
	{
		const FString uid = GetUserProducer()->GetClientUser()->id_;
		if (as_host)
		{
			SLobbyMember* member = lb->FindMember(uid);
			if (member && !member->is_host_)
				return false;
		}
		FString who(TEXT("{"));
		int to_who_num = 0;
		for (auto& member : lb->members_)
		{
			if (!member.user_id_.Equals(uid, ESearchCase::IgnoreCase))
			{
				who.Append(TEXT("'"));
				who.Append(member.user_id_);
				who.Append(TEXT("',"));
				to_who_num++;
			}
		}
		who.RemoveFromEnd(TEXT(","));
		who.Append(TEXT("}"));
		if (0 == msg.Find(LobbyFinishMark))
		{
			if (lb->LobbyStateMachine.Get())
			{
				lb->LobbyStateMachine->SetNextStateFunction("state_in_game");
				lb->LobbyStateMachine->ExplainCurrentFunction();
			}
			TPair<bool, TArray<FString>>* upr = UserPlayRoles.Find(msg.Mid(LobbyFinishMark.Len()));
			if (nullptr != upr)
				upr->Key = true;
			bool all_finish_select = true;
			for (auto& _upr : UserPlayRoles)
			{
				all_finish_select &= _upr.Value.Key;
			}
			if (CallbackActor)
			{
				CallbackActor->SetCallbackType(Callback_NotifyLobbyRoleSelectFinished);
				CallbackActor->NotifyLobbyRoleSelectFinished(all_finish_select);
			}
		}
		if (0 == to_who_num)
			return true;//don't need to forward to others.cause there's nobody.
		//build the request string for the server.
		FString to_server = FString::Printf(TEXT("{what = '%s',who=%s}"),
			*msg,*who);
		Instance->SendDataToSocketServer(FSocketProducer::DefaultSocketServerName,
			to_server, "forwardbyuserid");
		return true;
	}
	return false;
}

bool UTrainGameInstance::DispatchMsgToWorld(const FString & msg)
{
	FString who(TEXT("{}"));//to all.
	//build the request string for the server.
	FString to_server = FString::Printf(TEXT("{what = '%s',who=%s}"),
			*msg, *who);
	Instance->SendDataToSocketServer(FSocketProducer::DefaultSocketServerName,
			to_server, "forwardbyuserid");
	return true;
}

void UTrainGameInstance::AcceptActionStatusChange()
{
	TSharedPtr<FLobbyProducer>&& lp = GetLobbyProducer();
	SLobby* lb = lp->GetCurrentLobby();
	if (lb && TaskPool)
	{
		FString action_finish_desc;
		FString action_doing_desc;
		FString action_awake_desc;
		GetGameDescription(TEXT("action_finish"), action_finish_desc);
		GetGameDescription(TEXT("action_doing"), action_doing_desc);
		GetGameDescription(TEXT("action_awake"), action_awake_desc);
		for (auto& member : lb->members_)
		{
			for (auto& action : member.task_actions_)
			{
				STATIC_TASK_BASE::TaskItemActionPtr tap = TaskPool->GetSpecificTaskItemAction(
					action.task_id_,
					action.item_id_,
					action.action_id_);
				if (tap.Get())
				{
					if ((int)tap->state != action.action_status_)
					{
						FString msg;
						tap->state = (EBaseState)action.action_status_;
						msg.Append(TEXT("["));
						msg.Append(member.user_name_);
						msg.Append(TEXT("]:@"));
						RUNTIME_TASK_BASE::TaskPtr tp = TaskPool->GetSpecificTask(action.task_id_);
						if (tp.Get() && tp->bind_role_.Get())
						{
							msg.Append(tp->bind_role_->role_name_);
							msg.Append(TEXT(","));
						}
						if (BState_AWAKED == tap->state)
							msg.Append(action_awake_desc);
						else if (BState_ACTIVED == tap->state)
							msg.Append(action_doing_desc);
						else if (BState_FINISHED == tap->state)
							msg.Append(action_finish_desc);
						msg.Append(tap->action_name_);
						if (CallbackActor)
						{
							CallbackActor->SetCallbackType(Callback_NotifyScrollMessage);
							CallbackActor->NotifyScrollMessage(msg, true, 0.1f);
							//respond the effect of this action to the blueprint.
							CallbackActor->SetCallbackType(Callback_NotifyActionEffectReplication);
							CallbackActor->NotifyActionEffectReplication(action.task_id_, action.item_id_, action.action_id_, (EBaseState)action.action_status_);
						}
					}
				}
			}
		}
	}
}

void UTrainGameInstance::AcceptUserRoleSelectChange()
{
	TSharedPtr<FLobbyProducer>&& lp = GetLobbyProducer();
	SLobby* lb = lp->GetCurrentLobby();
	if (lb && TaskPool)
	{
		UserPlayRoles.Empty();
		for (auto& member : lb->members_)
		{
			TPair<bool,TArray<FString>>& r = UserPlayRoles.FindOrAdd(member.user_id_);
			for (auto& action : member.task_actions_)
			{
				RUNTIME_TASK_BASE::TaskPtr tp = TaskPool->GetSpecificTask(action.task_id_);
				if (tp.Get() && tp->bind_role_.Get())
					r.Value.AddUnique(tp->bind_role_->role_name_);
			}
		}
	}
}

void UTrainGameInstance::AddGameDescription(const FString & key, const FString & value)
{
	GameDescriptions.FindOrAdd(key) = value;
}

bool UTrainGameInstance::GetGameDescription(const FString & key, FString & value)
{
	value.Empty();
	const FString* v = GameDescriptions.Find(key);
	if (nullptr == v)
		return false;
	value = *v;
	return true;
}

ETaskActionPassbyResultType UTrainGameInstance::PassbyRoleTaskActionStatus(const int task_index, const int item_index, const int act_index, const int status)
{
	//notice the specific client
	//if this action can execute by self,just do it,don't need to forward.
	TSharedPtr<FLobbyProducer>&& lp = GetLobbyProducer();
	SLobby* lb = lp->GetCurrentLobby();
	if ( lb )
	{
		for (auto& member : lb->members_)
		{
			for (auto& action : member.task_actions_)
			{
				if (action.task_id_ == task_index
					&& action.item_id_ == item_index
					&& action.action_id_ == act_index)
				{
					if (member.user_id_.Equals(GetUserProducer()->GetClientUser()->id_, ESearchCase::IgnoreCase))
						return E_APRT_SELF_CAN_DO;
					FString who = FString::Printf(TEXT("{'%s'}"), *member.user_id_);//to the specific user.
					//build the request string for the server.
					FString to_server = FString::Printf(TEXT("{what = 'passbyroletaskaction|%d|%d|%d|%d',who=%s}"),
						task_index, item_index, act_index, status, *who);
					Instance->SendDataToSocketServer(FSocketProducer::DefaultSocketServerName,
						to_server, "forwardbyuserid");
					return E_APRT_OTHER_DO;
				}
			}
		}
	}
	return E_APRT_USER_NOT_FOUND;
}

void UTrainGameInstance::SendCharacterTransform(const FString & pawn_name)
{
	if( TrainMode == ETrainMode::TrainMode_Multi_Online_Player )
		AHumanCharacter::FSynchronizeSender::SyncToServer(pawn_name);
}

bool UTrainGameInstance::AcceptPassbyRoleTaskActionStatus(const FString & what)
{
	FString unpack = what;
	FString l, r;
	if (unpack.Split("passbyroletaskaction|", &l, &r))
	{
		FString _l, _r;
		int task_index = 0;
		int item_index = 0;
		int action_index = 0;
		int action_status = 0;
		if (r.Split("|", &_l, &_r))
		{
			task_index = FCString::Atoi(*_l);
			r = _r;
			if (r.Split("|", &_l, &_r))
			{
				item_index = FCString::Atoi(*_l);
				r = _r;
				if (r.Split("|", &_l, &_r))
				{
					action_index = FCString::Atoi(*_l);
					action_status = FCString::Atoi(*_r);
					if (CallbackActor)
					{
						CallbackActor->SetCallbackType(Callback_NotifyTaskCanPassbyRecieve);
						CallbackActor->NotifyTaskCanPassbyRecieve(task_index,item_index,action_index,(EBaseState)action_status);
						return true;
					}
				}
			}
		}
	}
	return false;
}

bool UTrainGameInstance::WrapperOfSocketPackageReciever(const uint8* data, const int32 len, const int32 nt)
{
	TSharedPtr<FSprotoPacker>&& lm = GetSprotoPacker();
	if (lm.Get())
	{
		if (SRNT_DATA == nt)
		{
			//using sproto to unpack
			//should be submit to the game thread do unpack these data.
			if (nullptr == data || 0 == len)
				return false;
			lm->NetSprotoUnPack(data, len);
		}
		else if (SRNT_RECONNECT_OK == nt || SRNT_RECONNECT_TRY == nt)
		{
			if (SRNT_RECONNECT_OK == nt)
			{
				//need to be reset.
			}
			if (UTrainGameInstance::Instance->CallbackActor)
				UTrainGameInstance::Instance->CallbackActor->SetServerReconnect((ESocketServerReconnect)nt);
		}
		return true;
	}
	return false;
}

void UTrainGameInstance::LobbyStateCallFunc(LobbyState * ls)
{
	if (nullptr == ls || nullptr == Instance )
		return;

	SLobby* lb = ls->lobby_host_;
	if (nullptr == lb)
		return;
	switch (LSM[ls->state_name_])
	{
	case LOBBY_CREATE_REQUESTING:
		{
			//build the request string for the server.
			FString to_server = FString::Printf(TEXT("{attr = %s}"),
				*lb->Serialize());
			Instance->SendDataToSocketServer(FSocketProducer::DefaultSocketServerName,
				to_server, "createlobby");
			//setup next symbol for the state machine.hosting.
			ls->lobby_host_->LobbyStateMachine->SetNextStateFunction(TEXT("state_hosting"));
		}
		break;
	case LOBBY_HOSTING:
		{
			//unlock the lobby,waiting for others to join in.
			int* a = (int*)&lb->averability_;
			FString to_server = FString::Printf(TEXT("{lobbyname='%s',stage=%d,averability=%d}"),
				*lb->name_, (int)LOBBY_HOSTING, *a);
			Instance->SendDataToSocketServer(FSocketProducer::DefaultSocketServerName,
				to_server, "updatelobby");
			if (UTrainGameInstance::Instance->CallbackActor)
			{
				UTrainGameInstance::Instance->CallbackActor->SetCallbackType(Callback_NotifyLobbyUpdate);
				UTrainGameInstance::Instance->CallbackActor->NotifyLobbyUpdate();
			}
		}
		break;
	case LOBBY_LAUNCHING:
		{
			//should launch.
			SLobbyMember* member = lb->FindMember(GetUserProducer()->GetClientUser()->id_);
			if (member)
			{
				member->stage_ = MEMBER_STAGE["r"];
				FString to_server = FString::Printf(TEXT("{lobbyname='%s',user_id='%s',stage='%s'}"),
					*lb->name_, *member->user_id_, *MEMBER_STAGE_REVERSE[member->stage_]);
				Instance->SendDataToSocketServer(FSocketProducer::DefaultSocketServerName,
					to_server, "updatememberstage");
				if (UTrainGameInstance::Instance->CallbackActor)
				{
					UTrainGameInstance::Instance->CallbackActor->SetCallbackType(Callback_NotifyLobbyUpdate);
					UTrainGameInstance::Instance->CallbackActor->NotifyLobbyUpdate();
				}
			}
		}
		break;
	case LOBBY_ROLE_SELECTING:
		{

		}
		break;
	case LOBBY_DESTROY:
		{

		}
		break;
	case LOBBY_JOIN_REQUESTING:
		{
			//build the request string for the server.
			SLobbyMember lm;
			lm.ability_ = GetUserProducer()->GetClientUser()->ability_;
			lm.is_host_ = false;
			lm.user_id_ = GetUserProducer()->GetClientUser()->id_;
			FString to_server = FString::Printf(TEXT("{lobbyname='matchup',who=%s}"),
				*lm.Serialize());
			Instance->SendDataToSocketServer(FSocketProducer::DefaultSocketServerName,
				to_server, "joinlobby");
		}
		break;
	case LOBBY_IN_GAME:
		{
			int* a = (int*)&lb->averability_;
			FString to_server = FString::Printf(TEXT("{lobbyname='%s',stage=%d,averability=%d}"),
				*lb->name_, (int)LOBBY_IN_GAME, *a);
			Instance->SendDataToSocketServer(FSocketProducer::DefaultSocketServerName,
				to_server, "updatelobby");
		}
		break;
	case LOBBY_HOST_CHANGING:
		{

		}
		break;
	case LOBBY_LOCKING:
		{
			//build the request string for the server.
			int* a = (int*)&lb->averability_;
			FString to_server = FString::Printf(TEXT("{lobbyname='%s',stage=%d,averability=%d}"),
				*lb->name_, (int)LOBBY_LOCKING,*a);
			Instance->SendDataToSocketServer(FSocketProducer::DefaultSocketServerName,
				to_server, "updatelobby");
		}
		break;
	case LOBBY_LEAVE:
		{
			//build the request string for the server.
			FString to_server = FString::Printf(TEXT("{lobbyname='%s',user_id='%s'}"),
				*lb->name_, *GetUserProducer()->GetClientUser()->id_);
			Instance->SendDataToSocketServer(FSocketProducer::DefaultSocketServerName,
				to_server, "leavelobby");
		}
		break;
	}
}
