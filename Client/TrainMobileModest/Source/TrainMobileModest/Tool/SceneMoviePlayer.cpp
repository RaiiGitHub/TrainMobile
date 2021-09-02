// As Part of GuangZhou Training.

#include "SceneMoviePlayer.h"
#include "TrainMobileModest.h"
#include "Runtime/MediaAssets/Public/MediaSource.h"


ASceneMoviePlayer::ASceneMoviePlayer()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bTickProcessMulti = false;
	bTickPlayStatus = true;
	MediaPlayer = nullptr;
}

// Called when the game starts or when spawned
void ASceneMoviePlayer::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ASceneMoviePlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//movie should be play in sequences.
	if (bTickProcessMulti && MovieSequence.Num() > 0)
	{
		if (MediaPlayer)
		{
			if (!bTickPlayStatus && MediaPlayer->IsPlaying())
				bTickPlayStatus = true;
			if( bTickPlayStatus && !MediaPlayer->IsPlaying() )
				PlayMovie();
		}
	}
}

void ASceneMoviePlayer::AcceptMediaPlayer(UMediaPlayer * const player)
{
	MediaPlayer = player;
}

void ASceneMoviePlayer::SetupMovie(const FString & movie_name)
{
	MovieSequence.Empty();
	MovieSequence.Add(FString::Printf(TEXT("FileMediaSource'/Game/Movies/%s.%s'"), *movie_name, *movie_name));
	bTickProcessMulti = false;
}

void ASceneMoviePlayer::SetupMovies(const TArray<FString>& movie_names)
{
	if (0 == movie_names.Num())
		return;
	MovieSequence.Empty();
	for (auto& mn : movie_names)
	{
		MovieSequence.Add(FString::Printf(TEXT("FileMediaSource'/Game/Movies/%s.%s'"), *mn, *mn));
	}
	bTickProcessMulti = false;
}

void ASceneMoviePlayer::PlayMovie()
{
	if (MediaPlayer && 0 != MovieSequence.Num())
	{
		UMediaSource* ms = LoadObject<UMediaSource>(NULL,
			*MovieSequence[0],
			NULL,
			LOAD_None,
			NULL);
		MediaPlayer->SetLooping(1 == MovieSequence.Num());
		MediaPlayer->OpenSource(ms);
		MediaPlayer->Play();
		MovieSequence.RemoveAt(0);
		bTickProcessMulti = true;
		bTickPlayStatus = false;
	}
	else
		bTickProcessMulti = false;
}

void ASceneMoviePlayer::StopMovie()
{
	if (MediaPlayer)
		MediaPlayer->Close();
	bTickProcessMulti = false;
	bTickPlayStatus = true;
}

void ASceneMoviePlayer::SeekTo(float percent)
{
	if (MediaPlayer)
	{
		FTimespan ts = MediaPlayer->GetDuration();
		FTimespan seekPos = FTimespan::FromMilliseconds(ts.GetTotalMilliseconds() * FMath::Clamp(percent,0.f,1.f));
		MediaPlayer->Seek(seekPos);
	}
}

UMediaPlayer * ASceneMoviePlayer::GetMediaPlayer()
{
	return MediaPlayer;
}