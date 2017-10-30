// As Part of GuangZhou Training.

#pragma once

#include "GameFramework/Actor.h"
#include "MovementAttacher.generated.h"

//handle the movement of the actor.
UCLASS()
class TRAINMOBILEMODEST_API AMovementAttacher : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMovementAttacher();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	struct MoveableActor {
		MoveableActor():actor_(nullptr), run_(false), go_to_first_point_state_(0), cur_go_to_first_point_len_(0.f), go_to_first_point_len_(0.f),can_patrol_(false), is_charactor_(false),cur_dir_(true), current_track_(nullptr),run_speed_(60.f), cur_track_point_index_(0), cur_seg_move_len_(0.f),cur_track_seg_len_(0.f){}
		bool operator < (const MoveableActor& bm) const
		{
			return actor_ < bm.actor_;
		}
		bool operator == (const MoveableActor& bm) const
		{
			return actor_ == bm.actor_;
		}
		AActor* actor_;
		TArray<FVector>* current_track_;
		bool run_;
		bool can_patrol_;//true for moving positive and negative.
		bool cur_dir_;//true for positive.
		bool is_charactor_;//will auto pass the velocity to the animation blueprint.
		int go_to_first_point_state_;//the state for going to the first point. 0-not done, 1-doing, 2-done.
		float cur_go_to_first_point_len_;//current movement of going to the first point.
		float go_to_first_point_len_;//total movements of going to the first point.
		FVector cur_go_to_first_point_dir_;//current movement direction of going to the first point.
		FVector cur_go_to_first_point_start_location_;//
		float run_speed_;//to determin the current run position.
		int cur_track_point_index_;//current index of the track points.
		float cur_seg_move_len_;//determin the exact position.
		float cur_track_seg_len_;//determin to change the points.
		FRotator pivot_rotation_;
	};
public:
	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	bool AddMovementActor(AActor* act,bool is_charactor = false,bool can_patrol = false, FRotator pivot_r = FRotator::ZeroRotator);

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	bool RemoveMovementActor(AActor* act);

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	bool DoMove(AActor* act,const FString& track_name,const float& at_what_speed);

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	bool DoStop(AActor* act);

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	bool SetPatrol(AActor* act,bool can_patrol = true);

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	bool AddMovementTrack(const FString& track_name,const TArray<FVector>& track_points);

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	bool RemoveMovementTrack(const FString& track_name);

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	bool SetPawnMovement(const FString& pawn_name, const FString& ani_name,const FTransform &tf,const FVector& velocity );

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	void SetupCallbackActor(class ACallbackActor* callback);

public:
	TMap<FString, TArray<FVector>> MovementTrack;
	TArray<MoveableActor> MovementActors;
private:
	class ACallbackActor* CallbackActor;
};
