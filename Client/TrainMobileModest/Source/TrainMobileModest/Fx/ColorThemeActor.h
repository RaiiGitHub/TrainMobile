// As Part of GuangZhou Training.

#pragma once

#include "GameFramework/Actor.h"
#include "DynamicMaterial.h"
#include "ColorThemeActor.generated.h"

UENUM(BlueprintType)
enum EColorThemeState
{
	ColorTheme_ApplyReady,
	ColorTheme_Applying,
	ColorTheme_ApplyPause,
	ColorTheme_ApplyComplete
};

UCLASS()
class TRAINMOBILEMODEST_API AColorThemeActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AColorThemeActor();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;
private:
	TArray<AActor*> LevelActors;
	int ApplyIndex;
	EColorThemeState ThemeApplyState;
	FName ScalarParamName;
	FName VectorParamName;
	FName TexParamName;
	FLinearColor ThemeColor;
	UMaterialInterface* MaterialInterface;
	DynamicMaterial Dm;
	float ColorChangeInterval;
public:
	//effects theme.
	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	int SetupLevelActors(float interval,float peak,FName tex_param_name, FName scalar_param_name, FName vector_param_name, FLinearColor color, UMaterialInterface* mi);

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	bool ApplyTheme();

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	void SetFixEnergy(float atWhatTime);

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	void SetDynamicEnergy();
};
