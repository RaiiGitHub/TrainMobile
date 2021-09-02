// As Part of GuangZhou Training.

#include "TextureFinder.h"
#include "TrainMobileModest.h"


// Sets default values
ATextureFinder::ATextureFinder()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	MethodSetName_ = TEXT("TextureFinder");

}

// Called when the game starts or when spawned
void ATextureFinder::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ATextureFinder::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
}

UTexture2D * ATextureFinder::LoadTexture(const FString & texPath)
{
	UTexture2D* findTex = LoadObject<UTexture2D>(NULL,
				*texPath,
				NULL,
				LOAD_None,
				NULL);
	if (findTex)
	{
		LoadedTextures_.Add(findTex);
	}
	return findTex;
}

FString ATextureFinder::GetMethodSetName() const
{
	return MethodSetName_;
}

void ATextureFinder::UnloadTexture()
{
	for (auto& tex : LoadedTextures_)
		tex->ReleaseResource();
	LoadedTextures_.Empty();
}

