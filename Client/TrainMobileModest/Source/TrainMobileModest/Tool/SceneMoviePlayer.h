// As Part of GuangZhou Training.

#pragma once

#include "GameFramework/Actor.h"
#include "MediaPlayer.h"
#include "SceneMoviePlayer.generated.h"

/**
 * 
 */
UCLASS()
class TRAINMOBILEMODEST_API ASceneMoviePlayer: public AActor
{
	GENERATED_BODY()
public:
	// Sets default values for this actor's properties
	ASceneMoviePlayer();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;
private:
	TArray<FString> MovieSequence;
	UMediaPlayer* MediaPlayer;
	bool bTickProcessMulti;
	bool bTickPlayStatus;
public:
	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	void AcceptMediaPlayer(UMediaPlayer* const player);

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	void SetupMovie(const FString& movie_name);

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	void SetupMovies(const TArray<FString>& movie_names);

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	void PlayMovie();

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	void StopMovie();

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	void SeekTo(float percent);

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	UMediaPlayer* GetMediaPlayer();
};
