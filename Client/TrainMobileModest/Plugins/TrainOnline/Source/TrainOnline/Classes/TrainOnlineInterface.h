#pragma once

class UHttpCompleteFunctionHolder;
class TRAINONLINE_API FTrainOnlineInterface
{
	friend class UHttpCompleteFunctionHolder;
public:
	FTrainOnlineInterface():HttpFunction(nullptr){}
	virtual ~FTrainOnlineInterface() {}
	enum LoginStatus
	{
		LSP_FAILED,
		LSP_SUCCESS,
		LSP_TOTAL
	};
public:
	void PutKeyValInPendingUser(const FString& key, const FString& val);
	void SetupHttpFunction(UObject* outer);
	void PutInHttpMethodDesc(const FString& desc);
	FString PopOutHttpMethodDesc(bool pop = true);
	UHttpCompleteFunctionHolder* GetHttpFunctionHolder() { return HttpFunction; }
protected:
	virtual void NotifyHttpRequestErr(const FString& msg, const FString& from_msg_type) {  }
	virtual void NotifyLoginResponse(LoginStatus ls) {}
	virtual void NotifyUserRankDataReady() {}
	virtual void NotifyUserIDExistVerify(const FString& id, bool exist) {}
	virtual void NotifyHandshake(const FString& msg, bool shake) {}
	virtual void NotifyRegisterResponse(bool success) {}
	virtual void NotifyUpdateUserExp() {}
	virtual void NotifyRespondUserCommand(const FString& cmd, bool exist) {}
	virtual void NotifyUserRankDataUploaded() {}

private:
	TQueue<FString> HttpMethodQueue;
	UHttpCompleteFunctionHolder* HttpFunction;
};