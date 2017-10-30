// As Part of GuangZhou Training.

#pragma once

#include "GameFramework/Actor.h"
#include "DisplayContent.generated.h"

UCLASS()
class TRAINMOBILEMODEST_API ADisplayContent : 
	public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADisplayContent();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;
private:
	TArray<FString> Content;
	bool bAutoAddTime;
	float WidgetWidth;
	float FontHeight;
public:
	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	void SetPageSize(int size);

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	void SetAutoAddTime(bool bAdd);

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	void SetWidgetWidth(float width);

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	void SetFontHeight(float font_height);

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	int AddContent(const FString& ct);

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	void ClearContent();

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	bool PagingContent(int pagingStep);

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	FString GetContent(int relativeIndex);//from 0

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	void MarkAsRead();

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	void Reset();

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	void WriteToLog(const FString& log);

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	FString SubStringByCharacterOtherwishAll(const FString& origin,const FString& sep);

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	FString AutoChangeLine(const FString& origin, const float word_size,const float line_maxwidth);

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	static void AutoWrapText(FString& out_format,int32& out_height, int32& out_width, const FString& origin, const float line_maxwidth,const FSlateFontInfo& fi);

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	bool AddPswContent(const FString& key,const FString& psw);

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	int RemovePswContent(const FString& key);

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	void ClearPswContent(const FString& key);

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	FString GetPswContent(const FString& key,bool encrypt);

public:
	//notify functions.
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "DispatchNotify")
	bool NotifyMessageReceived();
private:
	//paging
	int ContentPageIndex;
	int PageSize;
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = DisplayContent)
	int NewMsgNum;
private:
	//password text.
	TMap<FString,FString> MapPswContent;
};
