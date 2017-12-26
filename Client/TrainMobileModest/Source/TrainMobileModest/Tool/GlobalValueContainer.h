// As Part of GuangZhou Training.

#pragma once

#include "CoreMinimal.h"
#include "GlobalValueContainer.generated.h"

UCLASS()
class TRAINMOBILEMODEST_API UGlobalValueContainer : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
protected:
	static TMap<FString,AActor*>  GlobeActors;
	static TMap<FString, FString> SystemKeyValues;
	static TArray<AActor*>        MiniMapActors;
	static TArray<UUserWidget*>   UserWidgets;
	static FDateTime              GameBeginTime;
public:
	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	static void SetGameBeginTime(FDateTime dateTime);

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	static FDateTime& GetGameBeginTime();

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	static void AddGlobeActor(FString Key, AActor* pActor);

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	static void RemoveGlobeActor(FString Key);

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	static AActor* FindGlobeActor(FString Key);

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	static FString GetGlobeActorKey(AActor* pActor);

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	static void AssignSystemKeyValues();

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	static FString GetSystemValue(const FString& key);

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	static UUserWidget* CreateUserWidget(TSubclassOf<UUserWidget> widgetClass, bool bAddToViewport = true, int zOrder = 0);

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	static void AddWidgetToViewport(UUserWidget* widget, int zOrder = 0);

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	static void RemoveWidgetFromViewport(UUserWidget* widget);

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	static UUserWidget* GetWidgetFromViewport(const FString& logicName);

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	static void RemoveAllWidgetsFromViewport();

};
