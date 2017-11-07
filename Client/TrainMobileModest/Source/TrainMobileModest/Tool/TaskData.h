// As Part of GuangZhou Training.

#pragma once

#include "Engine/DataTable.h"
#include "TaskData.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum ETaskDataMetaType
{
    TaskDataMeta_ScriptTaskData,
    TaskDataMeta_TaskData,
	TaskDataMeta_RoleImageData,
	TaskDataMeta_RoleMovieData,
	TaskDataMeta_RoleFirstSight,
	TaskDataMeta_RoleGoodsTexture,
	TaskDataMeta_RoleTaskItemAction,
	TaskDataMeta_GroupLearningVideo,
};

USTRUCT(BlueprintType)
struct FScriptTaskDataStatistics
{
	GENERATED_USTRUCT_BODY()
public:
	FScriptTaskDataStatistics() {};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ScriptTaskDataStatistics)
	FString ScriptName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ScriptTaskDataStatistics)
	int TaskRoleNum;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ScriptTaskDataStatistics)
	int TaskNodeNum;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ScriptTaskDataStatistics)
	int TaskItemNum;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ScriptTaskDataStatistics)
	float SuggestCostTimeToPlay;
};

USTRUCT(BlueprintType)
struct FTaskDataMeta
{
	GENERATED_USTRUCT_BODY()
public:
	FTaskDataMeta() {};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = TaskDataMeta)
	FString MetaName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = TaskDataMeta)
	TEnumAsByte<ETaskDataMetaType> MetaType;
};

USTRUCT(BlueprintType)
struct FScriptTaskData : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
public:
	FScriptTaskData() {}
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ScriptTaskData)
	FString ScriptName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ScriptTaskData)
	TArray<FTaskDataMeta> TaskDataMeta;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ScriptTaskData)
	FString Memo;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ScriptTaskData)
	FString TypeName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ScriptTaskData)
	FString IconPath;
};

USTRUCT(BlueprintType)
struct FTaskData : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
public:
	FTaskData() {}
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = TaskData)
	FString Role;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = TaskData)
	FString Name;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = TaskData)
	TArray<FString> Content;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = TaskData)
	FString Position;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = TaskData)
	FString Action;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = TaskData)
	FString Memo;
};

USTRUCT(BlueprintType)
struct FRoleImageData : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
public:
	FRoleImageData() {}
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = RoleImageData)
	FString Role;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = RoleImageData)
	FString PreFix;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = RoleImageData)
	FString Path;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = RoleImageData)
	FString HeadPortraitPath;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = RoleImageData)
	int StartIndex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = RoleImageData)
	int EndIndex;

};

USTRUCT(BlueprintType)
struct FRoleMovieData : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
public:
	FRoleMovieData() {}
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = RoleMovieData)
	FString Role;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = RoleMovieData)
	TArray<FString> Movies;
};

USTRUCT(BlueprintType)
struct FRoleFirstSight : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
public:
	FRoleFirstSight() {}
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FRoleFirstSight)
	FString Role;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FRoleFirstSight)
	bool bEnable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FRoleFirstSight)
	float aspectRation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FRoleFirstSight)
	float fov;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FRoleFirstSight)
	FRotator sightRotation;

};

USTRUCT(BlueprintType)
struct FTaskItemTex {
	GENERATED_USTRUCT_BODY()
public:
	FTaskItemTex() {}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FTaskItemTex)
	FString TexPath;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FTaskItemTex)
	FString TexDesc;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FTaskItemTex)
	FString TexName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FTaskItemTex)
	int TaskNodeIndex;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FTaskItemTex)
	int TaskItemIndex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FTaskItemTex)
	int TaskItemActionIndex;
};

USTRUCT(BlueprintType)
struct FRoleGoodsTexture : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
public:
	FRoleGoodsTexture() {}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FRoleGoodsTexture)
	FString Role;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FRoleGoodsTexture)
	TArray<FTaskItemTex> Texs;
};

USTRUCT(BlueprintType)
struct FTaskItemAction {
	GENERATED_USTRUCT_BODY()
public:
	FTaskItemAction() {}

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FTaskItemAction)
	FString ActName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FTaskItemAction)
	FString ActDesc;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FTaskItemAction)
	FString ActResult;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FTaskItemAction)
	int TaskNodeIndex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FTaskItemAction)
	int TaskItemIndex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FTaskItemAction)
	int ActionIndex;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FTaskItemAction)
	int ActState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FTaskItemAction)
	bool ActRelateToLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FTaskItemAction)
	FVector ActLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FTaskItemAction)
	bool ActRelateToSpriteLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FTaskItemAction)
	FVector ActSpriteLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FTaskItemAction)
	FVector ActSpriteScale;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FTaskItemAction)
	int ActExpBenchmark;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FTaskItemAction)
	int ActExpActualDuration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FTaskItemAction)
	int ActSuggestOrder;
};

USTRUCT(BlueprintType)
struct FTaskItemActionStatistics {
	GENERATED_USTRUCT_BODY()
public:
	FTaskItemActionStatistics() {}
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FTaskItemActionStatistics)
	FString RoleName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FTaskItemActionStatistics)
	FString CurActionName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FTaskItemActionStatistics)
	float CurPeriodPlayTime;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FTaskItemActionStatistics)
	int CurPeriodExp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FTaskItemActionStatistics)
	int CurFinishedActionCount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FTaskItemActionStatistics)
	int CurTotalActionCount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FTaskItemActionStatistics)
	int CurTaskItemExp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FTaskItemActionStatistics)
	int CurTaskItemPlayTime;
};

USTRUCT(BlueprintType)
struct FRoleTaskItemAction : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
public:
	FRoleTaskItemAction() {}
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FRoleTaskItemAction)
	FString Role;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FRoleTaskItemAction)
	TArray<FTaskItemAction> Actions;
};

USTRUCT(BlueprintType)
struct FLearningVideo
{
	GENERATED_USTRUCT_BODY()
public:
	FLearningVideo() {}
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FLearningVideo)
	FString VideoPath;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FLearningVideo)
	FString VideoThumbnailPath;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FLearningVideo)
	FString VideoDesc;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FLearningVideo)
	FString VideoOtherInfo;
};

USTRUCT(BlueprintType)
struct FGroupLearningVideo: public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
public:
	FGroupLearningVideo() {}
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FGroupLearningVideo)
	FString GroupName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FGroupLearningVideo)
	FString GroupDesc;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FGroupLearningVideo)
	TArray<FLearningVideo> Videos;
};

USTRUCT(BlueprintType)
struct FTrackPoints : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
public:
	FTrackPoints() {}
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FTrackPoints)
		FString TrackName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FTrackPoints)
		TArray<FVector> Points;
};

USTRUCT(BlueprintType)
struct FTipCharacters : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
public:
	FTipCharacters() {}
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FTipCharacters)
		FString Key;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FTipCharacters)
		FString Value;
};
