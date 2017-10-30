// As Part of GuangZhou Training.

#pragma once

#include "GameFramework/Actor.h"
#include "StagePackHelper.generated.h"

UCLASS()
class TRAINMOBILEMODEST_API AStagePackHelper : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AStagePackHelper();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;
private:
	TArray<class UTexture2D*> PackTexs;
	int PageSize;
	int CurPage;
	int ActiveIndex;
public:
	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	void SetPageSize(int size);

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	int Paging(int pageStep);

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	int GetPageTex(TArray<class UTexture2D*>& texs);

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	bool SetActiveIndex(int index);

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	int GetActiveIndex();

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	bool MatchedTestWithActiveAction(int goodsIndex);

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	UTexture2D* GetActivePageTex();

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	int GetActiveIndexInPack();

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	void Reset();

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	bool AddPageTex(const FString& tex);
};
