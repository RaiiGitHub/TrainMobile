// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "TrainOnlineActor.h"
#include "TrainOnlinePrivatePCH.h"


ATrainOnlineActor* ATrainOnlineActor::Handler = nullptr;

ATrainOnlineActor::ATrainOnlineActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void ATrainOnlineActor::SetupHandler()
{
	Handler = this;
}

FUserInfo ATrainOnlineActor::GetUserInfo() const
{
	FUserInfo ur;
	const TSharedPtr<SUser>& u = GetUserProducer()->GetClientUser();
	if (u.Get())
	{
		ur.ID = u->id_;
		ur.PSW = u->psw_;
		ur.Name = u->name_;
		ur.LastLoginTime = u->last_login_time_;
		ur.OtherFiledValue = u->other_field_value_;
		ur.Exp = u->exp_;
		ur.ExpDuration = u->exp_duration_;
		ur.Rank = u->rank_;
		ur.Ability = u->ability_;
		ur.Version = u->version_;
		ur.InnerVersion = FUserProducer::InnerVersion;
		ur.SocketServerIp = u->socket_server_ip_;
		ur.SocketServerPort = u->socket_server_port_;
	}
	return ur;
}

FString ATrainOnlineActor::GetHttpServerAddr() const
{
	const TSharedPtr<FHttpWorker>& h = GetUserProducer()->GetHttpWorker();
	if (h.Get())
		return h->HttpServerAddress;//here will not be guaranteed that whether exist or not.
	return FString();
}

FString ATrainOnlineActor::GetSokcetServerName() const
{
	return FSocketProducer::DefaultSocketServerName;
}

int ATrainOnlineActor::ExpToLevel(int exp) const
{
	//y = aln(b*x)
	double a = 2.f;
	double b = 1.f;
	const TSharedPtr<SUser>& u = GetUserProducer()->GetClientUser();
	if (u.Get())
	{
		a = u->exp_to_lv_param1_;
		b = u->exp_to_lv_param2_;
	}
	return FPlatformMath::FloorToInt(a * FMath::Loge(FMath::Max(EULERS_NUMBER,(float)b * exp)));
}

void ATrainOnlineActor::PutKeyValInPendingUser(const FString & key, const FString & val)
{
	TSharedPtr<SUser>& u = GetUserProducer()->GetClientUser();
	if (u.Get())
	{
		if (0 == key.Compare(TEXT("psw"), ESearchCase::IgnoreCase))
			u->psw_ = val;
		else if (0 == key.Compare(TEXT("id"), ESearchCase::IgnoreCase))
			u->id_ = val;
		else if (0 == key.Compare(TEXT("name"), ESearchCase::IgnoreCase))
			u->name_ = val;
		else
			u->other_field_value_.Add(key,val);
	}
}

FLobbyInfo ATrainOnlineActor::GetCurrentLobby() const
{
	FLobbyInfo li;
	SLobby* lb = GetLobbyProducer()->GetCurrentLobby();
	if (lb)
	{
		li.Name = lb->name_;
		li.AverAbility = lb->averability_;
		for (auto& member : lb->members_)
		{
			FMemberInfo mi;
			mi.ID = member.user_id_;
			mi.Name = member.user_name_;
			mi.Status = (ELobbyMemberStatus)member.stage_;
			mi.IsHost = member.is_host_;
			li.Members.Add(mi);
		}
	}
	return li;
}

ATrainOnlineActor * ATrainOnlineActor::GetHandler()
{
	return ATrainOnlineActor::Handler;
}

void ATrainOnlineActor::HttpCompleteCallback(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	FString MessageBody = "";
	SUser& u = *GetUserProducer()->GetClientUser().Get();
	const FString method_desc = PopOutHttpMethodDesc();
	// If HTTP fails client-side, this will still be called but with a NULL shared pointer!
	if (!Response.IsValid())
	{
		MessageBody = TEXT("{\"success\":\"Error: Unable to process HTTP Request!\"}");
		if (method_desc.Equals(TEXT("user_login")))
			u.logining_ = false;
		if (Handler)
			Handler->NotifyHttpRequestErr(FString::Printf(TEXT("%s-Error: Unable to process HTTP Request!"), *method_desc));//to blueprint.
		return;
	}
	else if (EHttpResponseCodes::IsOk(Response->GetResponseCode()))
	{
		MessageBody = Response->GetContentAsString();
	}
	else
	{
		MessageBody = FString::Printf(TEXT("{\"success\":\"HTTP Error: %d\"}"), Response->GetResponseCode());
		if (method_desc.Equals(TEXT("user_login")))
			u.logining_ = false;
		if (Handler)
			Handler->NotifyHttpRequestErr(FString::Printf(TEXT("%s-Error: method not support."), *method_desc));//to blueprint.
		return;
	}

	// Create the Json parser
	TSharedPtr<FJsonObject> JsonObject;
	TSharedRef<TJsonReader<> > JsonReader = TJsonReaderFactory<>::Create(MessageBody);
	
	if (FJsonSerializer::Deserialize(JsonReader, JsonObject) &&
		JsonObject.IsValid())
	{
		TSharedPtr<FJsonValue> respond_code = JsonObject->TryGetField(TEXT("code"));
		TSharedPtr<FJsonValue> respond_method = JsonObject->TryGetField(TEXT("method"));
		if (respond_code.Get()
			&& respond_method.Get())
		{
			const FString& rm = respond_method->AsString();
			bool rc = respond_code->AsBool();
			if (rm.Equals(TEXT("user_login")))
			{
				//login
				if( !rc )
					u.logining_ = false;
				if (Handler)
					Handler->NotifyLoginResponse(rc?LS_PERMISSION_SUCCESS: LS_PERMISSION_FAILED);//to blueprint.
				if (rc)
				{
					//read user-detail
					u.permission_ = true;
					TSharedPtr<FJsonValue> respond_result = JsonObject->TryGetField(TEXT("result"));
					if (respond_result.Get())
					{
						const TSharedPtr<FJsonObject>* result_object;
						if (respond_result->TryGetObject(result_object))
						{
							//will read the version as well.
							for (auto& kv : (*result_object)->Values)
							{
								if (kv.Key.Equals(TEXT("rank"), ESearchCase::IgnoreCase))
									kv.Value->TryGetNumber(u.rank_);
								else if (kv.Key.Equals(TEXT("last_login_time"), ESearchCase::IgnoreCase))
									kv.Value->TryGetString(u.last_login_time_);
								else if (kv.Key.Equals(TEXT("name"), ESearchCase::IgnoreCase))
									kv.Value->TryGetString(u.name_);
								else if (kv.Key.Equals(TEXT("id"), ESearchCase::IgnoreCase))
									kv.Value->TryGetString(u.id_);
								else if (kv.Key.Equals(TEXT("psw"), ESearchCase::IgnoreCase))
									kv.Value->TryGetString(u.psw_);
								else if (kv.Key.Equals(TEXT("exp"), ESearchCase::IgnoreCase))
									kv.Value->TryGetNumber(u.exp_);
								else if (kv.Key.Equals(TEXT("exp_duration"), ESearchCase::IgnoreCase))
									kv.Value->TryGetNumber(u.exp_duration_);
								else if (kv.Key.Equals(TEXT("ability"), ESearchCase::IgnoreCase))
									kv.Value->TryGetNumber(u.ability_);
								else if (kv.Key.Equals(TEXT("version"), ESearchCase::IgnoreCase))
									kv.Value->TryGetString(u.version_);
								else if (kv.Key.Equals(TEXT("apk_url"), ESearchCase::IgnoreCase))
									kv.Value->TryGetString(u.apk_url_);
								else if (kv.Key.Equals(TEXT("exp_to_level_param1"), ESearchCase::IgnoreCase))
									kv.Value->TryGetNumber(u.exp_to_lv_param1_);
								else if (kv.Key.Equals(TEXT("exp_to_level_param2"), ESearchCase::IgnoreCase))
									kv.Value->TryGetNumber(u.exp_to_lv_param2_);
								else if (kv.Key.Equals(TEXT("socket_server_ip"), ESearchCase::IgnoreCase))
									kv.Value->TryGetString(u.socket_server_ip_);
								else if (kv.Key.Equals(TEXT("socket_server_port"), ESearchCase::IgnoreCase))
									kv.Value->TryGetNumber(u.socket_server_port_);
								else if (kv.Key.Equals(TEXT("download_tips"), ESearchCase::IgnoreCase))
									kv.Value->TryGetString(u.download_tips_);
								else if (kv.Key.Equals(TEXT("no_wifi_tips"), ESearchCase::IgnoreCase))
									kv.Value->TryGetString(u.no_wifi_tips_);
								else if (kv.Key.Equals(TEXT("download_caption"), ESearchCase::IgnoreCase))
									kv.Value->TryGetString(u.download_caption_);
								else
									u.other_field_value_.Add(kv.Key, kv.Value->AsString());
							}
						}
						//notify blueprint
						if (Handler)
							Handler->NotifyLoginResponse(LS_PERMISSION_TOTAL);
					}
				}
			}
			else if (rm.Equals(TEXT("user_get_info")))
			{
				TSharedPtr<FJsonValue> respond_result = JsonObject->TryGetField(TEXT("result"));
				if (respond_result.Get())
				{
					const TSharedPtr<FJsonObject>* result_object;
					if (respond_result->TryGetObject(result_object))
					{
						SLobby* lobby = GetLobbyProducer()->GetCurrentLobby();
						TSharedPtr<FJsonValue> uid = (*result_object)->TryGetField(TEXT("id"));
						if (lobby && uid.Get())
						{
							SLobbyMember* member = lobby->FindMember(uid->AsString());
							if (member)
							{
								for (auto& kv : (*result_object)->Values)
								{
									if (kv.Key.Equals(TEXT("name"), ESearchCase::IgnoreCase))
										kv.Value->TryGetString(member->user_name_);
									else if (kv.Key.Equals(TEXT("ability"), ESearchCase::IgnoreCase))
										kv.Value->TryGetNumber(member->cur_ability_);
									else if (kv.Key.Equals(TEXT("exp"), ESearchCase::IgnoreCase))
										kv.Value->TryGetNumber(member->cur_exp_);
								}
								if (method_desc.Equals(TEXT("respond_user_rank_result")))
								{
									member->rank_data_ready_ = true;
									//notify blueprint
									if (Handler && lobby->CheckRankReady())
										Handler->NotifyUserRankDataReady();
								}
							}
						}
					}
				}
			}
			else if (rm.Equals(TEXT("version")))
			{
				TSharedPtr<FJsonValue> respond_result = JsonObject->TryGetField(TEXT("result"));
				if (respond_result.Get())
				{
					const TSharedPtr<FJsonObject>* result_object;
					if (respond_result->TryGetObject(result_object))
					{
						TSharedPtr<FJsonValue> version_val = (*result_object)->TryGetField(TEXT("version"));
						if (version_val.Get())
							u.version_ = version_val->AsString();
						
						TSharedPtr<FJsonValue> apk_url_val = (*result_object)->TryGetField(TEXT("apk_url"));
						if (version_val.Get())
							u.apk_url_ = apk_url_val->AsString();

						TSharedPtr<FJsonValue> exp_to_lv_param1_val = (*result_object)->TryGetField(TEXT("exp_to_level_param1"));
						if (version_val.Get())
							u.exp_to_lv_param1_ = exp_to_lv_param1_val->AsNumber();

						TSharedPtr<FJsonValue> exp_to_lv_param2_val = (*result_object)->TryGetField(TEXT("exp_to_level_param2"));
						if (version_val.Get())
							u.exp_to_lv_param2_ = exp_to_lv_param2_val->AsNumber();

						TSharedPtr<FJsonValue> socket_sever_ip_val = (*result_object)->TryGetField(TEXT("socket_server_ip"));
						if (version_val.Get())
							u.socket_server_ip_ = socket_sever_ip_val->AsString();

						TSharedPtr<FJsonValue> socket_sever_port_val = (*result_object)->TryGetField(TEXT("socket_server_port"));
						if (version_val.Get())
							u.socket_server_port_ = (int32)socket_sever_port_val->AsNumber();

						TSharedPtr<FJsonValue> download_tips_val = (*result_object)->TryGetField(TEXT("download_tips"));
						if (version_val.Get())
							u.download_tips_ = download_tips_val->AsString();

						TSharedPtr<FJsonValue> no_wifi_tips_val = (*result_object)->TryGetField(TEXT("no_wifi_tips"));
						if (version_val.Get())
							u.no_wifi_tips_ = no_wifi_tips_val->AsString();

						TSharedPtr<FJsonValue> download_caption_val = (*result_object)->TryGetField(TEXT("download_caption"));
						if (version_val.Get())
							u.download_caption_ = download_caption_val->AsString();
					}
				}
			}
			else if (rm.Equals(TEXT("user_id_exist_verify")))
			{
				TSharedPtr<FJsonValue> respond_user_id = JsonObject->TryGetField(TEXT("id"));
				if ( Handler && respond_user_id.Get())
					Handler->NotifyUserIDExistVerify(respond_user_id->AsString(), rc);
			}
			else if (rm.Equals(TEXT("handshake")))
			{
				if (Handler)
					Handler->NotifyHandshake(method_desc,rc);
			}
			else if (rm.Equals(TEXT("user_register")))
			{
				if (Handler)
					Handler->NotifyRegisterMsg(rc);
			}
			else if (rm.Equals(TEXT("user_update_info")))
			{
				if (Handler)
					Handler->NotifyUpdateUserExp();
			}
			else if (rm.Equals(TEXT("respond_command")))
			{
				TSharedPtr<FJsonValue> respond_cmd = JsonObject->TryGetField(TEXT("result"));
				if (Handler && respond_cmd.Get())
					NotifyRespondUserCommand(respond_cmd->AsString(),rc);
			}
			else if (rm.Equals(TEXT("user_upload_team")))
			{
				if (Handler)
					Handler->NotifyUserRankDataUploaded();
			}
			//others
		}
	}
}

void ATrainOnlineActor::PutInHttpMethodDesc(const FString & desc)
{
	//method may probably come in in a very short time. so here's a list as a queue.
	HttpMethodQueue.Enqueue(desc);
}

FString ATrainOnlineActor::PopOutHttpMethodDesc(bool pop)
{
	FString desc;
	pop?HttpMethodQueue.Dequeue(desc): HttpMethodQueue.Peek(desc);
	return desc;
}
