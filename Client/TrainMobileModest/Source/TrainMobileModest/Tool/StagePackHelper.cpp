// As Part of GuangZhou Training.

#include "StagePackHelper.h"
#include "TrainMobileModest.h"
#include "TextureFinder.h"
#include "TaskPool.h"


// Sets default values
AStagePackHelper::AStagePackHelper()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	PageSize = 6;
	CurPage = 1;//start from 1
	ActiveIndex = -1;//start from 0,-1 is not been actived yet.
}

// Called when the game starts or when spawned
void AStagePackHelper::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AStagePackHelper::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

void AStagePackHelper::SetPageSize(int size)
{
	PageSize = size;
}

int AStagePackHelper::Paging(int pageStep)
{
	int contentLen = PackTexs.Num();
	int pageNum = FPlatformMath::CeilToInt((float)contentLen / (float)PageSize);
	if (CurPage + pageStep > pageNum || CurPage + pageStep < 1)
		return 0;
	CurPage += pageStep;
	ActiveIndex = -1;
	return 0;
}

int AStagePackHelper::GetPageTex(TArray<class UTexture2D*>& texs)
{
	int startIndex = (CurPage - 1)*PageSize;
	int lenTex = PackTexs.Num();
	int getCount = 0;
	for (int i = startIndex; i < lenTex; ++i)
	{
		texs.Add(PackTexs[i]);
		++getCount;
		if (getCount == PageSize)
			return getCount;
	}	
	return getCount;
}

bool AStagePackHelper::SetActiveIndex(int index)
{
	if (index == -1)
	{
		ActiveIndex = index;
		return false;
	}
	if (index >= PageSize || index < 0)
		return false;
	int lenCheckMin = (CurPage - 1)*PageSize ;
	if (index < lenCheckMin || index > lenCheckMin + PageSize)
		return false;
	ActiveIndex = index;
	return true;
}

int AStagePackHelper::GetActiveIndex()
{
	return ActiveIndex;
}

bool AStagePackHelper::MatchedTestWithActiveAction(int goodsIndex)
{
	ATaskPool* taskPool = static_cast<ATaskPool*>(FindObject<ATaskPool>(ANY_PACKAGE, TEXT("TaskPool")));
	if (nullptr == taskPool)
		return false;
	const FTaskItemTex& item = taskPool->GetCurrentTaskItemGoods(goodsIndex);
	return item.TaskItemActionIndex == taskPool->GetCurrentTaskItemActionIndex();
}

UTexture2D* AStagePackHelper::GetActivePageTex()
{
	int indexTex = GetActiveIndexInPack();
	if (indexTex < 0 || indexTex >= PackTexs.Num())
		return nullptr;
	return PackTexs[indexTex];
}

int AStagePackHelper::GetActiveIndexInPack()
{
	if (-1 == ActiveIndex)
		return -1;
	return ActiveIndex + (CurPage - 1)*PageSize;
}

void AStagePackHelper::Reset()
{
	CurPage = 1;//start from 1
	ActiveIndex = -1;//start from 0,-1 is not been actived yet.
	PackTexs.Empty();
}

bool AStagePackHelper::AddPageTex(const FString& tex)
{
	ATextureFinder* texFinder = static_cast<ATextureFinder*>(FindObject<AActor>(ANY_PACKAGE, TEXT("TextureFinder")));
	if (nullptr == texFinder)
		return false;
	UTexture2D* tex2D = texFinder->LoadTexture(tex);
	if (nullptr != tex2D)
	{
		PackTexs.AddUnique(tex2D);
		return true;
	}
	return false;
}

