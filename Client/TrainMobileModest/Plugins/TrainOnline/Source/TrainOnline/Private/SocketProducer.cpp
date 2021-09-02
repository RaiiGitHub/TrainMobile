#include "SocketProducer.h"
#include "TrainOnlinePrivatePCH.h"
#include "UserProducer.h"
#include "Sockets.h"
#include "SocketSubsystem.h"
#include "IPAddress.h"
#include "IPv4/IPv4SubnetMask.h"
#include "IPv4/IPv4Address.h"
#include "Misc/ScopeLock.h"
#include "LuaProvider.h"

FString FSocketProducer::DefaultSocketServerName = TEXT("qianqiansocketserver");


FSocketProducer::FSocketProducer()
{
}


FSocketProducer::~FSocketProducer()
{
}

STrainSocket * FSocketProducer::CreateSocket(const FString& socket_desc, bool force_UDP)
{
	for (auto& ts : TrainSocektListPool)
	{
		if (ts.Get() && ts->socket_desc_.Equals(socket_desc))
			return ts.Get();
	}
	//new
	TrainSocketPtr tsp = MakeShareable(new STrainSocket);
	tsp->socket_ = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateSocket(NAME_Stream, socket_desc, force_UDP);
	tsp->force_UDP_ = force_UDP;
	tsp->socket_desc_ = socket_desc;
	TrainSocektListPool.AddTail(tsp);
	return tsp.Get();
}

STrainSocket * FSocketProducer::FindSocket(const FString & socket_desc)
{
	for (auto& ts : TrainSocektListPool)
	{
		if (ts.Get() && ts->socket_desc_.Equals(socket_desc))
			return ts.Get();
	}
	return nullptr;
}

TrainSocketPtr* FSocketProducer::GetSocketPtr(const FString & socket_desc)
{
	for (auto& ts : TrainSocektListPool)
	{
		if (ts.Get() && ts->socket_desc_.Equals(socket_desc))
			return &ts;
	}
	return nullptr;
}

bool FSocketProducer::DisConnect(STrainSocket * ts)
{
	if (nullptr == ts || nullptr == ts->socket_ )
		return false;
	ts->socket_->Close();
	ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(ts->socket_);
	ts->socket_ = nullptr;
	for (auto& sk : TrainSocektListPool)
	{
		if (sk.Get() && sk.Get() == ts)
		{
			TrainSocektListPool.RemoveNode(sk);
			return true;
		}
	}
	return false;
}

bool FSocketProducer::ConnectToServer(STrainSocket* ts, const FString & server_ip, int32 server_port)
{
	if (nullptr == ts || nullptr == ts->socket_ )
		return false;
	FIPv4Address ip;
	FIPv4Address::Parse(server_ip, ip);
	TSharedRef<FInternetAddr> addr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
	addr->SetIp(ip.Value);
	addr->SetPort(server_port);
	bool con = ts->socket_->Connect(*addr);
	GEngine->AddOnScreenDebugMessage(
		-1,
		20.f,
		con ? FColor::Green : FColor::Red,
		FString::Printf(TEXT("FSocketProducer::ConnectToServer ~>%s %s"), *addr->ToString(true), con ?TEXT("connected."):TEXT("failed.")));
	return con;
}
												    
bool FSocketProducer::SendDataToServer(STrainSocket * ts, const FString & content)
{
	if (nullptr == ts || nullptr == ts->socket_ || ts->reconnecting_ )
		return false;
	const TCHAR *serializedChar = content.GetCharArray().GetData();
	if (nullptr == serializedChar)
		return false;
	int32 size = FCString::Strlen(serializedChar);
	int32 sent = 0;
	return ts->socket_->Send((uint8*)TCHAR_TO_UTF8(serializedChar), size, sent);
}

bool FSocketProducer::SendBinaryDataToServer(STrainSocket * ts, const char * buf, int32 len)
{
	if( nullptr == buf || 0 >= len || nullptr == ts->socket_ || ts->reconnecting_ )
		return false;
	int32 sent = 0;
	return ts->socket_->Send((uint8*)buf, len, sent);
}

bool FSocketProducer::SetupSocketReciever(STrainSocket * ts, RecvDelegateFunc func)
{
	//called in a thread.
	if (nullptr == ts || nullptr == ts->socket_ || nullptr == func)
		return false;
	if (ts->socket_data_reciever_.Get())
		return false;
	if (FPlatformProcess::SupportsMultithreading())
	{
		auto _ts = GetSocketProducer()->GetSocketPtr(ts->socket_desc_);
		ts->socket_data_reciever_ = MakeShareable(new FSocketDataReciever(*_ts, func));
		ts->socket_data_sender_ = MakeShareable(new FSocketDataSender(*_ts,this, GetSprotoPacker().Get()));
		return true;
	}
	return false;
}

FSocketDataReciever::FSocketDataReciever(TSharedPtr<STrainSocket, ESPMode::ThreadSafe>& ts, RecvDelegateFunc func)
	:bReadyToCheckHeartbeat(false)
	, HangDuration(25.0)
	, ReconnectingTries(0)
{
	TrainSocket = ts;
	DelegateFunc = func;
	Thread = MakeShareable(FRunnableThread::Create(this, *FString::Printf(TEXT("SocketDataReciever-%u"), this)));
}

FSocketDataReciever::~FSocketDataReciever()
{
	Stop();
}

uint32 FSocketDataReciever::Run()
{
	if (!TrainSocket.IsValid() || nullptr == DelegateFunc )
		return 0;
	FPlatformProcess::Sleep(0.03);
	//While not told to stop this thread 
	while ( !IsFinished() )
	{
		if (bReadyToCheckHeartbeat)
		{
			if (!IsBeating())
			{
			   //ooops! It may be dead. Need to be reconnected.
				while (!IsFinished())
				{
					if (ReConnect())//try to reconnect all the time?other thread can stop it by checking the ReconnectingTries.
					{
						//handshake again.
						const TSharedPtr<SUser>& u = GetUserProducer()->GetClientUser();
						if (ATrainOnlineActor::GetHandler())
							ATrainOnlineActor::GetHandler()->NotifySocketServerConnected(u->id_, FSocketProducer::DefaultSocketServerName, true);
						break;
					}
					FPlatformProcess::Sleep(0.5f);
				}
			}
		}
		if (TrainSocket.IsValid()
			&& TrainSocket.Pin()->socket_
			&& !TrainSocket.Pin()->reconnecting_)
		{
			uint32 DatagramSize = 0;
			while (TrainSocket.IsValid()
				&& TrainSocket.Pin()->socket_
				&& TrainSocket.Pin()->socket_->HasPendingData(DatagramSize))
			{
				TSharedPtr<FArrayReader, ESPMode::ThreadSafe> Datagram = MakeShareable(new FArrayReader(true));
				Datagram->SetNumUninitialized(FMath::Min(DatagramSize, 65507u));
				int32 BytesRead = 0;
				if (TrainSocket.Pin()->socket_->Recv(Datagram->GetData(), Datagram->Num(), BytesRead))
				{
					UE_LOG(TrainOnlinePlugin, Log, TEXT("FSocketDataReciever::Run~>Recieve data len:%d"), BytesRead);
					DelegateFunc(Datagram->GetData(), BytesRead, SRNT_DATA);//process right now!
				}
			}
		}
		FPlatformProcess::Sleep(0.01f);
	}
	return 0;
}

void FSocketDataReciever::Stop()
{
	bReadyToCheckHeartbeat = false;
	FRunableWorker::Stop();
}

void FSocketDataReciever::StartHeartBeat()
{
	bReadyToCheckHeartbeat = true;
}

void FSocketDataReciever::HeartBeat()
{
	FScopeLock HeartBeatLock(&HeartBeatCritical);
	LastHeartBeatTime = FPlatformTime::Seconds();
}

bool FSocketDataReciever::IsBeating()
{
	// Check heartbeat
	const double CurrentTime = FPlatformTime::Seconds();
	FScopeLock HeartBeatLock(&HeartBeatCritical);
	return 	CurrentTime - LastHeartBeatTime < HangDuration;
}

bool FSocketDataReciever::IsReadyToBeat()
{
	return bReadyToCheckHeartbeat;
}

bool FSocketDataReciever::ReConnect()
{
	if (!TrainSocket.IsValid())
		return false;
	bReadyToCheckHeartbeat = false;
	FScopeLock ReconnectLock(&HeartBeatCritical);
	const TSharedPtr<SUser>& u = GetUserProducer()->GetClientUser();
	FIPv4Address ip;
	FIPv4Address::Parse(u->socket_server_ip_, ip);
	TSharedRef<FInternetAddr> addr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
	addr->SetIp(ip.Value);
	addr->SetPort(u->socket_server_port_);
	if(!TrainSocket.Pin()->reconnecting_)
		TrainSocket.Pin()->socket_->Close();
	ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(TrainSocket.Pin()->socket_);
	TrainSocket.Pin()->socket_ = nullptr;
	TrainSocket.Pin()->socket_ = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateSocket(NAME_Stream, TrainSocket.Pin()->socket_desc_, TrainSocket.Pin()->force_UDP_);
	bool con = TrainSocket.Pin()->socket_->Connect(*addr);
	u->sokcet_server_permission_ = con;
	ReconnectingTries = con ? 0 : (ReconnectingTries + 1);
	TrainSocket.Pin()->reconnecting_ = !con;
	if (DelegateFunc)
	{
		//try to notify game instance need to reset something.
		DelegateFunc(0, 0, con ? SRNT_RECONNECT_OK : SRNT_RECONNECT_TRY);
	}
	if (GEngine)
	{
		UE_LOG(TrainOnlinePlugin, Log, TEXT("FSocketDataReciever::Run ~>Server reconnecting.%s[%s]-try %d"),
			*addr->ToString(true),
			con ? TEXT("connected.") : TEXT("failed."),
			ReconnectingTries);
		//GEngine->AddOnScreenDebugMessage(
		//	-1,
		//	20.f,
		//	con?FColor::Green: FColor::Red,
		//	FString::Printf(TEXT("FSocketDataReciever::Run ~>Server reconnecting.%s[%s]-try %d"), 
		//		*addr->ToString(true),
		//		con ? TEXT("connected.") : TEXT("failed."),
		//		ReconnectingTries));
	}
	return con;
}

///////
TSharedPtr<FSocketEnvWorker> FSocketEnvWorker::SocketEnvWorker;

FSocketEnvWorker::FSocketEnvWorker(FUserProducer* up, FSocketProducer* sp)
{
	UserProducer = up;
	SocketProducer = sp;
	Thread = MakeShareable(FRunnableThread::Create(this, *FString::Printf(TEXT("FSocketEnvWorker-%u"), this)));
}

FSocketEnvWorker::~FSocketEnvWorker()
{
	Stop();
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			20.f,
			FColor::Green,
			FString::Printf(TEXT("FSocketEnvWorker::~FSocketEnvWorker ~> destruct.")));
	}
}

uint32 FSocketEnvWorker::Run()
{
	//run by the created thread at the construct function.
	if ( UserProducer && SocketProducer )
	{
		const TSharedPtr<SUser>& u = UserProducer->GetClientUser();
		if (u.Get() && !u->socket_server_ip_.IsEmpty())
		{
			const FString socket_name(FSocketProducer::DefaultSocketServerName);
			STrainSocket* ts = SocketProducer->CreateSocket(socket_name, false);
			bool con = SocketProducer->ConnectToServer(ts, u->socket_server_ip_, u->socket_server_port_);
			u->sokcet_server_permission_ = con;
			if (con && ATrainOnlineActor::GetHandler())
				ATrainOnlineActor::GetHandler()->NotifySocketServerConnected(u->id_,socket_name,false);
		}
	}
	GEngine->AddOnScreenDebugMessage(
		-1,
		20.f,
		FColor::Green,
		FString::Printf(TEXT("FSocketEnvWorker::Run ~> finished.")));
	return 0;
}

//////////////
FSocketDataSender::FSocketDataSender(TSharedPtr<STrainSocket,ESPMode::ThreadSafe>& ts, FSocketProducer * sp, FSprotoPacker* packer)
{
	TrainSocket = ts;
	SocketProducer = sp;
	SprotoPacker = packer;
	Thread = MakeShareable(FRunnableThread::Create(this, *FString::Printf(TEXT("SocketDataSender-%u"), this)));
	ThreadEvent = FPlatformProcess::GetSynchEventFromPool(false);
}

FSocketDataSender::~FSocketDataSender()
{
	Stop();
	FPlatformProcess::ReturnSynchEventToPool(ThreadEvent);
	ThreadEvent = nullptr;
	SocketProducer = nullptr;
	SprotoPacker = nullptr;
}

uint32 FSocketDataSender::Run()
{
	if (nullptr == SocketProducer|| nullptr == SprotoPacker )
		return 0;
	FPlatformProcess::Sleep(0.03f);
	while (true)
	{
		ThreadEvent->Wait();
		for (int i = 0; i < 10; ++i )
		{
			TSharedPtr<FSendBuffer, ESPMode::ThreadSafe> e = SendBuffer[i];
			if (e.IsValid())
			{
				SendBuffer[i].Reset();
				if (TrainSocket.IsValid())
				{
					//try to using sprotopacker.
					FSprotoPacker::FSenderBufferPtr&& buf = SprotoPacker->NetSprotoPack(e->proto_request, e->content);
					if (buf.Get())
						SocketProducer->SendBinaryDataToServer(TrainSocket.Pin().Get(), buf->buf, buf->len);
				}
			}
		}
	}
	return 0;
}

void FSocketDataSender::PushBuffer(const FString & content, const FString& proto_request)
{
	TSharedPtr<FSendBuffer, ESPMode::ThreadSafe> buffer = MakeShareable(new FSendBuffer);
	buffer->content = content;
	buffer->proto_request = proto_request;
	if (!TrainSocket.IsValid()
		|| nullptr == TrainSocket.Pin()->socket_
		|| TrainSocket.Pin()->reconnecting_)
	{
		return;
	}
	for (int i = 0; i < 10; ++i)
	{
		if (!SendBuffer[i].IsValid())
		{
			SendBuffer[i] = buffer;
			ThreadEvent->Trigger();
			return;
		}
	}
}

