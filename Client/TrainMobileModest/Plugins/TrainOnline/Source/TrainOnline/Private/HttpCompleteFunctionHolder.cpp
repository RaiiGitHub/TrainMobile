#include "HttpCompleteFunctionHolder.h"
#include "TrainOnlinePrivatePCH.h"
#include "TrainOnlineInterface.h"

void UHttpCompleteFunctionHolder::HttpCompleteCallback(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (nullptr == LogicHolder)
		return;
	FString MessageBody = "";
	SUser& u = *GetUserProducer()->GetClientUser().Get();
	const FString method_desc = LogicHolder->PopOutHttpMethodDesc();
	// If HTTP fails client-side, this will still be called but with a NULL shared pointer!
	if (!Response.IsValid())
	{
		MessageBody = TEXT("{\"success\":\"Error: Unable to process HTTP Request!\"}");
		if (method_desc.Equals(TEXT("user_login")))
			u.logining_ = false;

		LogicHolder->NotifyHttpRequestErr(FString::Printf(TEXT("%s-Error: Unable to process HTTP Request!"), *method_desc), method_desc);
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
			
		LogicHolder->NotifyHttpRequestErr(FString::Printf(TEXT("%s-Error: method not support."), *method_desc), method_desc);
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
				if (!rc)
				{
					u.logining_ = false;
					LogicHolder->NotifyLoginResponse(FTrainOnlineInterface::LSP_FAILED);
				}
				else
				{
					//read user-detail
					u.permission_ = true;
					LogicHolder->NotifyLoginResponse(FTrainOnlineInterface::LSP_SUCCESS);
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
						LogicHolder->NotifyLoginResponse(FTrainOnlineInterface::LSP_TOTAL);
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
									if (lobby->CheckRankReady())
										LogicHolder->NotifyUserRankDataReady();
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
				if (respond_user_id.Get())
					LogicHolder->NotifyUserIDExistVerify(respond_user_id->AsString(), rc);
			}
			else if (rm.Equals(TEXT("handshake")))
			{
				LogicHolder->NotifyHandshake(method_desc, rc);
			}
			else if (rm.Equals(TEXT("user_register")))
			{
				LogicHolder->NotifyRegisterResponse(rc);
			}
			else if (rm.Equals(TEXT("user_update_info")))
			{
				LogicHolder->NotifyUpdateUserExp();
			}
			else if (rm.Equals(TEXT("respond_command")))
			{
				TSharedPtr<FJsonValue> respond_cmd = JsonObject->TryGetField(TEXT("result"));
				if (respond_cmd.Get())
					LogicHolder->NotifyRespondUserCommand(respond_cmd->AsString(), rc);
			}
			else if (rm.Equals(TEXT("user_upload_team")))
			{
				LogicHolder->NotifyUserRankDataUploaded();
			}
			//others
		}
	}
}
