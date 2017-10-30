// As Part of GuangZhou Training.

#include "TrainMobileModest.h"
#include "ColorThemeActor.h"
#include "DynamicMaterialOnMesh.h"


// Sets default values
AColorThemeActor::AColorThemeActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	ThemeApplyState = ColorTheme_ApplyReady;
	ApplyIndex = 0;
	ColorChangeInterval = 0.f;
}

// Called when the game starts or when spawned
void AColorThemeActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AColorThemeActor::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
	if (ColorTheme_Applying == ThemeApplyState)
	{
		ColorChangeInterval += DeltaTime;
		//if (ColorChangeInterval >= 0.2f)
		{
			if (ApplyIndex < LevelActors.Num())
			{
				Dm.BeginWithAllElement(LevelActors[ApplyIndex]->GetName(), TexParamName,ScalarParamName, VectorParamName, ThemeColor, MaterialInterface);
				ApplyIndex++;
			}
			else
			{
				ThemeApplyState = ColorTheme_ApplyComplete;
			}
			ColorChangeInterval = 0.f;
		}
	}
	Dm.Tick(DeltaTime);
}

int AColorThemeActor::SetupLevelActors(float interval, float peak, FName tex_param_name,FName scalar_param_name, FName vector_param_name, FLinearColor color, UMaterialInterface* mi)
{
	LevelActors.Empty();
	FConstLevelIterator li =  GetWorld()->GetLevelIterator();
	for (; li; li++)
	{
		for (auto& act : (*li)->Actors)
		{
			if (act && act->GetClass())
			{
				const FString cn = act->GetClass()->GetName();
				if (cn.Equals(TEXT("StaticMeshActor"), ESearchCase::IgnoreCase)
					|| cn.Equals(TEXT("HumanMaster_C"), ESearchCase::IgnoreCase))
				{
					LevelActors.Add(act);
				}
				else if (cn.Equals(TEXT("DynamicMaterialOnMesh")))
				{
					ADynamicMaterialOnMesh* dmom = static_cast<ADynamicMaterialOnMesh*>(act);
					dmom->Restore(FString());
					dmom->Reset();
				}
			}
		}
	}
	//

	Dm.Reset();
	Dm.SetupBreathMethod(interval, peak);
	ThemeApplyState = ColorTheme_ApplyReady;
	ScalarParamName = scalar_param_name;
	VectorParamName = vector_param_name;
	TexParamName = tex_param_name;
	ThemeColor = color;
	MaterialInterface = mi;
	ApplyIndex = 0;
	return LevelActors.Num();
}

bool AColorThemeActor::ApplyTheme()
{
	ThemeApplyState = ColorTheme_Applying;
	//ADirectionalLight* dl = FindObject<ADirectionalLight>(ANY_PACKAGE, TEXT("DirectionalLight"));
	//if (dl)
	//{
	//	UDirectionalLightComponent* dlc = static_cast<UDirectionalLightComponent*>(dl->GetLightComponent());
	//	if (dlc)
	//		dlc->SetLightColor(ThemeColor);
	//}
	////skylight
	//ASkyLight* sl = FindObject<ASkyLight>(ANY_PACKAGE, TEXT("SkyLight"));
	//if (sl)
	//{
	//	USkyLightComponent* slc = sl->GetLightComponent();
	//	if (slc)
	//		slc->SetLightColor(ThemeColor);
	//}
	return true;
}

void AColorThemeActor::SetFixEnergy(float atWhatTime)
{
	Dm.SetFixEnergy(atWhatTime);
}

void AColorThemeActor::SetDynamicEnergy()
{
	Dm.SetDynamicEnergy();
}


