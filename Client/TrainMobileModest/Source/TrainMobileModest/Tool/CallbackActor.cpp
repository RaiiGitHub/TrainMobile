// As Part of GuangZhou Training.

#include "TrainMobileModest.h"
#include "CallbackActor.h"
#include "PawnHandler.h"
#include "TrainGameInstance.h"


// Sets default values
ACallbackActor::ACallbackActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	CallbackType = Callback_Unkown;
}

// Called when the game starts or when spawned
void ACallbackActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACallbackActor::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
	UTrainGameInstance::HandleSprotoDataFromQueue();
	if (ServerReconnect > E_RECONNECT_NO_NEED)
	{
		NotifySocketServerReconnect(ServerReconnect);
		ServerReconnect = E_RECONNECT_NO_NEED;
	}
}

void ACallbackActor::SetCallbackType(ECallbackType type)
{
	CallbackType = type;
}

void ACallbackActor::SetServerReconnect(const ESocketServerReconnect & ssr)
{
	ServerReconnect = ssr;
}

bool ACallbackActor::GetCallbackDataHitResult(FHitResult & hr)
{
	if (Callback_NotifyHitResult != CallbackType)
		return false;
	APawnHandler* ph = FindObject<APawnHandler>(ANY_PACKAGE, TEXT("PawnHandler"));
	if (ph)
		return false;
	return ph->GetHitResult(hr);
}

ECallbackType ACallbackActor::GetCallbackType()
{
	return CallbackType;
}

