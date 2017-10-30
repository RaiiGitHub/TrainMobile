#pragma once

#include "RunableWorker.h"

enum ESocketReieverNotifyType {
	SRNT_DATA = 0,
	SRNT_RECONNECT_TRY = 1,
	SRNT_RECONNECT_OK = 2
};
typedef bool(*RecvDelegateFunc)(const uint8* data,const int32 len,const int32 nt);
struct STrainSocket;
class TRAINONLINE_API FSocketDataReciever
	:public FRunableWorker {
public:
	FSocketDataReciever(TSharedPtr<STrainSocket, ESPMode::ThreadSafe>& ts, RecvDelegateFunc func);
	~FSocketDataReciever();
	virtual uint32 Run() override;
	virtual void Stop() override;
public:
	/** Time we last received a heartbeat for the current thread */
	double LastHeartBeatTime;
	/** Max time the thread is allowed to not send the heartbeat*/
	double HangDuration;
	/** Reconnecting tries.**/
	int ReconnectingTries;
	/** True if heartbeat should be measured */
	FThreadSafeBool bReadyToCheckHeartbeat;
	/** Synch object for the heartbeat */
	FCriticalSection HeartBeatCritical;
public:
	/** Begin measuring heartbeat */
	void StartHeartBeat();
	/** heartbeat at this thread*/
	void HeartBeat();
	/** Returns true/false if this thread is currently performing heartbeat monitoring */
	bool IsBeating();
	/** Ready to beat? */
	bool IsReadyToBeat();
	/** reconnect this socket*/
	bool ReConnect();
private:
	RecvDelegateFunc DelegateFunc;
	TWeakPtr<STrainSocket, ESPMode::ThreadSafe> TrainSocket;
};

class FSocketProducer;
class FSprotoPacker;
class TRAINONLINE_API FSocketDataSender
	:public FRunableWorker {
public:
	FSocketDataSender(TSharedPtr<STrainSocket,ESPMode::ThreadSafe>& ts,FSocketProducer* sp,FSprotoPacker* packer);
	~FSocketDataSender();
	virtual uint32 Run() override;
public:
	struct FSendBuffer
	{
		FString content;
		FString proto_request;
	};
	void PushBuffer(const FString & content, const FString& proto_request);
private:
	FSocketProducer* SocketProducer;
	FSprotoPacker* SprotoPacker;
	FEvent* ThreadEvent;
	TWeakPtr<STrainSocket,ESPMode::ThreadSafe> TrainSocket;
	TSharedPtr<FSendBuffer, ESPMode::ThreadSafe> SendBuffer[10];
};

class FUserProducer;
//connect to the socket server
class TRAINONLINE_API FSocketEnvWorker
		:public FRunableWorker {
public:
	FSocketEnvWorker(FUserProducer* up, FSocketProducer* sp);
	~FSocketEnvWorker();
	virtual uint32 Run() override;
private:
	FUserProducer* UserProducer;
	FSocketProducer* SocketProducer;
public:
	static TSharedPtr<FSocketEnvWorker> SocketEnvWorker;
};

struct STrainSocket {
	class FSocket* socket_;
	FString socket_desc_;
	bool force_UDP_;
	bool reconnecting_;
	TSharedPtr<FSocketDataReciever> socket_data_reciever_;
	TSharedPtr<FSocketDataSender> socket_data_sender_;
	bool operator < (const STrainSocket& ts) const
	{
		return socket_ < ts.socket_;
	}
	bool operator == (const STrainSocket& ts) const
	{
		return socket_ == ts.socket_;
	}
	STrainSocket() :socket_(nullptr), force_UDP_(false), reconnecting_(false){}
	~STrainSocket(){}
};
typedef TSharedPtr<STrainSocket,ESPMode::ThreadSafe> TrainSocketPtr;
typedef TDoubleLinkedList<TrainSocketPtr> TrainSocketList;
typedef TDoubleLinkedList<TrainSocketPtr>::TDoubleLinkedListNode TrainSocketNode;



class TRAINONLINE_API FSocketProducer
{
public:
	FSocketProducer();
	~FSocketProducer();
private:
	TrainSocketList TrainSocektListPool;
public:
	static FString DefaultSocketServerName;
public:
	STrainSocket* CreateSocket(const FString& socket_desc,bool force_UDP = false);
	STrainSocket* FindSocket(const FString& socket_desc);
	TrainSocketPtr* GetSocketPtr(const FString& socket_desc);
	bool DisConnect(STrainSocket* ts);
	bool ConnectToServer(STrainSocket* ts,const FString& server_ip, int32 server_port);
	bool SendDataToServer(STrainSocket* ts,const FString& content);
	bool SendBinaryDataToServer(STrainSocket* ts, const char* buf, int32 len);
	bool SetupSocketReciever(STrainSocket * ts, RecvDelegateFunc func);
};

