// As Part of GuangZhou Training.

#include "TrainMobileModest.h"
#include "MovementAttacher.h"
#include "HumanCharacter.h"
#include "CallbackActor.h"


// Sets default values
AMovementAttacher::AMovementAttacher()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AMovementAttacher::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AMovementAttacher::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	for (auto& actor : MovementActors)
	{
		if (actor.actor_ && actor.run_ && actor.current_track_)
		{
			static FVector location;
			float movement = actor.run_speed_ * DeltaTime;//m/s.
			bool location_ok = false;
			float run_speed = actor.run_speed_;

			if ( 2 != actor.go_to_first_point_state_)
			{
				//first should go to the first point.
				if (0 == actor.go_to_first_point_state_)
				{
					actor.cur_go_to_first_point_start_location_ = actor.actor_->GetActorLocation();
					((*actor.current_track_)[actor.cur_track_point_index_]
						- actor.cur_go_to_first_point_start_location_)
						.ToDirectionAndLength(actor.cur_go_to_first_point_dir_,actor.go_to_first_point_len_);
					actor.cur_go_to_first_point_len_ = 0.f;
					actor.go_to_first_point_state_ = 1;//doing
				}
				actor.cur_go_to_first_point_len_ += movement;
				if (actor.cur_go_to_first_point_len_ > actor.go_to_first_point_len_)
				{
					actor.cur_go_to_first_point_len_ = 0.f;
					actor.go_to_first_point_state_ = 2;
				}
				else
				{
					location_ok = true;
					location = actor.cur_go_to_first_point_start_location_ + actor.cur_go_to_first_point_dir_ * actor.cur_go_to_first_point_len_;
				}
			}
			else
			{
				//go to the first point done.
				actor.cur_seg_move_len_ += movement;
				int next_index = actor.cur_dir_ ? actor.cur_track_point_index_ + 1 : actor.cur_track_point_index_ - 1;
				if (actor.cur_track_seg_len_ <= actor.cur_seg_move_len_)
				{
					//need to move to the next point.
					actor.cur_seg_move_len_ = 0.f;
					if (actor.cur_dir_ && next_index == actor.current_track_->Num() - 1)
					{
						//need to go back.
						actor.cur_dir_ = false;
						location = (*actor.current_track_)[next_index];
						actor.run_ = actor.can_patrol_;
						location_ok = true;
						actor.cur_track_point_index_ = next_index;
						next_index -= 1;
						run_speed = 0.f;
						if (!actor.run_ && CallbackActor)
						{
							CallbackActor->SetCallbackType(Callback_NotifyActorMovementDone);
							CallbackActor->NotifyActorMovementDone(actor.actor_);
						}
					}
					else if (!actor.cur_dir_ && next_index == 0)
					{
						//need to go again.
						actor.cur_dir_ = true;
						location = (*actor.current_track_)[next_index];
						actor.run_ = actor.can_patrol_;
						location_ok = true;
						actor.cur_track_point_index_ = next_index;
						next_index += 1;
						run_speed = 0.f;
						if (!actor.run_ && CallbackActor)
						{
							CallbackActor->SetCallbackType(Callback_NotifyActorMovementDone);
							CallbackActor->NotifyActorMovementDone(actor.actor_);
						}
					}
					else
					{
						location = (*actor.current_track_)[next_index];
						actor.cur_track_point_index_ = next_index;
						next_index = actor.cur_dir_ ? next_index + 1 : next_index - 1;
						location_ok = true;
					}
					//to the next.
					actor.cur_track_seg_len_ = FVector::Distance((*actor.current_track_)[next_index],
						(*actor.current_track_)[actor.cur_track_point_index_]);
				}
				else if (0 <= next_index && actor.current_track_->Num() > next_index)
				{
					//normal location
					FVector dl = (*actor.current_track_)[next_index]
						- (*actor.current_track_)[actor.cur_track_point_index_];
					FVector dir;
					float len = 0.f;
					dl.ToDirectionAndLength(dir, len);
					location = (*actor.current_track_)[actor.cur_track_point_index_] + dir*actor.cur_seg_move_len_;
					location_ok = true;
				}
			}
			if (location_ok)
			{
				FVector cur_dl = location - actor.actor_->GetActorLocation();
				FVector cur_dir;
				float cur_len = 0.f;
				cur_dl.ToDirectionAndLength(cur_dir, cur_len);
				float ha = cur_dir.HeadingAngle();
				ha = FMath::RadiansToDegrees(ha);
				FRotator r = actor.actor_->GetActorRotation();
				r.Yaw = actor.pivot_rotation_.Yaw + ha;
				actor.actor_->SetActorRotation(r);
				actor.actor_->SetActorLocation(location);
				if (actor.is_charactor_)
				{
					AHumanCharacter* character = static_cast<AHumanCharacter*>(actor.actor_);
					character->VelocityIgnoreCollision = run_speed;
				}
			}
		}
	}
}

bool AMovementAttacher::AddMovementActor(AActor * act,  bool is_charactor,bool can_patrol, FRotator pivot_r)
{
	if (nullptr == act)
		return false;
	MoveableActor ma;
	ma.actor_ = act;
	int32 index = MovementActors.Find(ma);
	if (index == INDEX_NONE)
	{
		ma.can_patrol_ = can_patrol;
		ma.pivot_rotation_ = pivot_r;
		ma.is_charactor_ = is_charactor;
		MovementActors.AddUnique(ma);
		return true;
	}
	return false;
}

bool AMovementAttacher::RemoveMovementActor(AActor * act)
{
	MoveableActor ma;
	ma.actor_ = act;
	int32 index = MovementActors.Find(ma);
	if (index != INDEX_NONE)
	{
		MovementActors.RemoveAt(index);
		return true;
	}
	return false;
}

bool AMovementAttacher::DoMove(AActor * act, const FString & track_name, const float& at_what_speed)
{
	MoveableActor ma;
	ma.actor_ = act;
	int32 index = MovementActors.Find(ma);
	TArray<FVector>* track = MovementTrack.Find(track_name);
	if (nullptr == track || 2 > track->Num() )
		return false;
	if (index != INDEX_NONE)
	{
		MoveableActor& actor = MovementActors[index];
		actor.run_ = true;
		actor.run_speed_ = at_what_speed;
		actor.cur_dir_ = true;
		actor.cur_track_point_index_ = 0;
		actor.cur_seg_move_len_ = 0.f;
		actor.current_track_ = track;
		actor.go_to_first_point_state_ = 0;
		actor.cur_go_to_first_point_len_ = 0.f;
		//first segment.
		actor.cur_track_seg_len_ = FVector::Distance((*track)[0], (*track)[1]);
		return true;
	}
	return false;
}

bool AMovementAttacher::DoStop(AActor * act)
{
	MoveableActor ma;
	ma.actor_ = act;
	int32 index = MovementActors.Find(ma);
	if (index != INDEX_NONE)
	{
		MovementActors[index].run_ = false;
		MovementActors[index].go_to_first_point_state_ = 0;
		MovementActors[index].cur_go_to_first_point_len_ = 0.f;
		return true;
	}
	return false;
}

bool AMovementAttacher::SetPatrol(AActor * act, bool can_patrol)
{
	MoveableActor ma;
	ma.actor_ = act;
	int32 index = MovementActors.Find(ma);
	if (index != INDEX_NONE)
	{
		MovementActors[index].can_patrol_ = can_patrol;
		return true;
	}
	return false;
}

bool AMovementAttacher::AddMovementTrack(const FString & track_name, const TArray<FVector>& track_points)
{
	if (MovementTrack.Find(track_name))
	{
		//already exist.
		return false;
	}
	MovementTrack.Add(track_name, track_points);
	return true;
}

bool AMovementAttacher::RemoveMovementTrack(const FString & track_name)
{
	return INDEX_NONE != MovementTrack.Remove(track_name);
}

bool AMovementAttacher::SetPawnMovement(const FString & pawn_name, const FString& ani_name, const FTransform &tf, const FVector& velocity)
{
	APawn* pawn = FindObject<APawn>(ANY_PACKAGE, *pawn_name);
	if (nullptr == pawn)
		return false;
	pawn->GetMovementComponent()->Modify();
	pawn->GetMovementComponent()->Velocity = velocity;
	pawn->SetActorTransform(tf);
	return false;
}

void AMovementAttacher::SetupCallbackActor(class ACallbackActor * callback)
{
	CallbackActor = callback;
}

