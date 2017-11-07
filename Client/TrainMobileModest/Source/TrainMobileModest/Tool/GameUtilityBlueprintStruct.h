// As Part of GuangZhou Training.

#pragma once
#include "GameUtilityBlueprintStruct.generated.h"
/**
 * 
 */
UENUM(BlueprintType)
enum ETrainMode
{
	TrainMode_Single_Player,
	TrainMode_Multi_Online_Player
};

USTRUCT(BlueprintType)
struct FLobbyBasicInfo
{
	GENERATED_USTRUCT_BODY()
public:
	FLobbyBasicInfo() {};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = LobbyBasicInfo)
		FString LobbyName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = LobbyBasicInfo)
		FString TaskScriptName;
};

USTRUCT(BlueprintType)
struct FUserAsRoles
{
	GENERATED_USTRUCT_BODY()
public:
	FUserAsRoles() {};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FUserAsRoles)
		FString UserID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FUserAsRoles)
		bool IsLocal;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FUserAsRoles)
		TArray<FString> RoleNames;
};

UENUM(BlueprintType)
enum ESocketServerReconnect
{
	E_RECONNECT_NO_NEED = 0,
	E_RECONNECT_TRY = 1,
	E_RECONNECT_OK = 2
};

UENUM(BlueprintType)
enum ETalkMessageType
{
	E_MSG_WORLD = 1,
	E_MSG_TEAM = 2
};

UENUM(BlueprintType)
enum ETaskActionPassbyResultType
{
	E_APRT_SELF_CAN_DO = 1,
	E_APRT_OTHER_DO = 2,
	E_APRT_USER_NOT_FOUND =3
};

UENUM(BlueprintType)
enum EScriptState
{
	EScriptState_Active = 1,
	EScriptState_Inactive = 2,
	EScriptState_Unvalid = 3
};

USTRUCT(BlueprintType)
struct FGameKeyValue
{
	GENERATED_USTRUCT_BODY()
public:
	FGameKeyValue() {};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FGameKeyValue)
		FString Key;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FGameKeyValue)
		FString Value;
};

///
class GameUtilityTool {
public:
	static bool GetValueInGameKeyValue(const TArray<FGameKeyValue>& kv, const FString& k,FString& v)
	{
		v.Empty();
		for (auto& _kv : kv)
		{
			if (k.Equals(_kv.Key))
			{
				v = _kv.Value;
				return true;
			}
		}
		return false;
	}
};