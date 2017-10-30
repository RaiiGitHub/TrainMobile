#pragma once
#include "LuaMachine.h"

namespace sproto {
	class sproto_base;
};

//sender and reciver.
//using luamachine standalone instance.
/**
* Delegate called when there's any recieved data in the queue.
*/
//DECLARE_DELEGATE(FSprotoDataRecieveDelegate);

class LUAPROVIDER_API FSprotoPacker
{
public:
	FSprotoPacker();
	~FSprotoPacker();
public:
	typedef FLuaMachine::FLuaBinaryBufferPtr FSenderBufferPtr;
	typedef TSharedPtr<sproto::sproto_base,ESPMode::ThreadSafe> FSprotoBufferPtr;
public:
	void Initialize();
	bool NetSprotoSetup(FLuaMachine* lvm,const FString lib = TEXT("sprotolib"));
	TArray<FSprotoBufferPtr> GetRecieverData();
	void RecieverPutData(const FSprotoBufferPtr& d);
	void NetSprotoUnPack(const uint8* pack, const int32 len);
	FSenderBufferPtr NetSprotoPack(const FString& proto_request, const FString& str_to_pack);
public:
	static TSharedPtr<FLuaMachine> LuaMachine;
	static FSprotoPacker* SprotoPackerIns;
private:
	FCriticalSection RecieverBuffCritical;
	FSprotoBufferPtr RecieverData[10];//cache sproto data from lua
};

