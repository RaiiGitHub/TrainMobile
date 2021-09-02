// As Part of GuangZhou Training.

#include "ActorTransformAni.h"
#include "TrainMobileModest.h"
#include "TaskPool.h"
#include "PawnHandler.h"
#include "CallbackActor.h"

static const float ata_x = 200.f;
static const float ata_y = 200.f;
// Sets default values
AActorTransformAni::AActorTransformAni()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AActorTransformAni::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AActorTransformAni::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
	//
	AniPointing(DeltaTime);
	AniLocating(DeltaTime);
	AniScaling(DeltaTime);
	AniRotating(DeltaTime);
	ReachMonitor(DeltaTime);
}

ActorTransformWrapper & AActorTransformAni::GetWrapperKey(const FString & name)
{
	// TODO: insert return statement here
	static ActorTransformWrapper key;
	if (nullptr == key.Aspect.Get())
		key.Aspect = MakeShareable(new FActorTransformAspect());
	key.Aspect->AspectName = name;
	int index = TransformAspects.Find(key);
	if (INDEX_NONE == index)
	{
		key.Aspect->AspectName.Empty();
		return key;
	}
	return TransformAspects[index];
}

void AActorTransformAni::AniLocating(float DeltaSeconds)
{
	for (auto& aspect : TransformAspects)
	{
		if (aspect.Aspect.Get() 
			&& TransformAspect_Location == aspect.Aspect->AspectType 
			&& aspect.Aspect->bRun)
		{
			//moving towards the direction from start length to end length.
			//step length
			float aniLen = aspect.Aspect->EndAniValue - aspect.Aspect->StartAniValue;
			float step = aniLen * DeltaSeconds / aspect.Aspect->Interval ;
			aspect.Aspect->CurAniValue += step;
			if (aspect.Aspect->CurAniValue >= aniLen)
			{
				//matched max length
				if (!aspect.Aspect->bCycle)
				{
					aspect.Aspect->CurAniValue = aniLen;
					continue;
				}
				step -= aspect.Aspect->CurAniValue - aniLen;
				step *= aspect.Aspect->bReverse ? -1 : 1;
				aspect.Aspect->bReverse = !aspect.Aspect->bReverse;
				aspect.Aspect->CurAniValue = 0.f;
			}
			else
				step *= aspect.Aspect->bReverse ? -1 : 1;

			for (auto& mesh : aspect.Aspect->AspectParams)
			{
				if (mesh.enable && mesh.mesh )
				{
					FVector curLoaction = mesh.mesh->GetActorLocation();
					curLoaction += aspect.Aspect->AniDirection * step;
					mesh.mesh->SetActorLocation(curLoaction);
				}
			}
		}
	}
}

void AActorTransformAni::AniRotating(float DeltaSeconds)
{
	for (auto& aspect : TransformAspects)
	{
		if (aspect.Aspect.Get() 
			&& TransformAspect_Rotation == aspect.Aspect->AspectType
			&& aspect.Aspect->bRun)
		{
			//rotating from start angle to end angle.
			//step angle
			//normaly do not need to be apply reversed.
			float aniLen = aspect.Aspect->EndAniValue - aspect.Aspect->StartAniValue;
			float step = aniLen * DeltaSeconds / aspect.Aspect->Interval;
			aspect.Aspect->CurAniValue += step;
			if (aspect.Aspect->CurAniValue >= aniLen)
			{
				//matched max length
				if (!aspect.Aspect->bCycle)
				{
					aspect.Aspect->CurAniValue = aniLen;
					continue;
				}
				step -= aspect.Aspect->CurAniValue - aniLen;
				//step *= aspect.Aspect->bReverse ? -1 : 1;
				aspect.Aspect->bReverse = !aspect.Aspect->bReverse;
				aspect.Aspect->CurAniValue = 0.f;
			}
			//else
				//step *= aspect.Aspect->bReverse ? -1 : 1;

			for (auto& mesh : aspect.Aspect->AspectParams)
			{
				if (mesh.enable && mesh.mesh)
				{
					FVector accVal = aspect.Aspect->AniDirection * step;
					FRotator rotate = mesh.mesh->GetActorRotation();
					rotate.Pitch += accVal.Y;
					rotate.Roll += accVal.X;
					rotate.Yaw += accVal.Z;
					mesh.mesh->SetActorRotation(rotate);
					rotate = mesh.mesh->GetActorRotation();
				}
			}
		}
	}
}

void AActorTransformAni::AniScaling(float DeltaSeconds)
{
	//scaling.
	for (auto& aspect : TransformAspects)
	{
		if (aspect.Aspect.Get()
			&& TransformAspect_Scale == aspect.Aspect->AspectType
			&& aspect.Aspect->bRun)
		{
			//rotating from start scale to end scale.
			//step angle
			float aniLen = aspect.Aspect->EndAniValue - aspect.Aspect->StartAniValue;
			float step = aniLen * DeltaSeconds / aspect.Aspect->Interval;
			aspect.Aspect->CurAniValue += step;
			if (aspect.Aspect->CurAniValue >= aniLen)
			{
				//matched max length
				if (!aspect.Aspect->bCycle)
				{
					aspect.Aspect->CurAniValue = aniLen;
					continue;
				}
				step -= aspect.Aspect->CurAniValue - aniLen;
				step *= aspect.Aspect->bReverse ? -1 : 1;
				aspect.Aspect->bReverse = !aspect.Aspect->bReverse;
				aspect.Aspect->CurAniValue = 0.f;
			}
			else
				step *= aspect.Aspect->bReverse ? -1 : 1;

			for (auto& mesh : aspect.Aspect->AspectParams)
			{
				if (mesh.enable && mesh.mesh)
				{
					FVector curScale = mesh.mesh->GetActorScale3D();
					curScale += aspect.Aspect->AniDirection * step;
					mesh.mesh->SetActorScale3D(curScale);
				}
			}
		}
	}
}

void AActorTransformAni::AniPointing(float DeltaSeconds)
{
	APawnHandler* ph = FindObject<APawnHandler>(ANY_PACKAGE, TEXT("PawnHandler"));
	if (nullptr == ph)
		return;
	ATaskPool* tp = FindObject<ATaskPool>(ANY_PACKAGE, TEXT("TaskPool"));
	if (nullptr == tp)
		return;
	APawn* pawn = ph->GetCurrentHandlePawn();
	if (nullptr == pawn)
		return;
	for (auto& aspect : TransformAspects)
	{
		if (aspect.Aspect.Get()
			&& TransformAspect_Pointer == aspect.Aspect->AspectType
			&& aspect.Aspect->bRun)
		{
			//always pointing the the target,target comes from the location of an activing action.
			for (auto& mesh : aspect.Aspect->AspectParams)
			{
				if (mesh.enable && mesh.mesh)
				{
					int cur_awake_act_index = tp->GetCurrentTaskItemAwakeActionIndex();
					if (0 == cur_awake_act_index)
						return;
					FVector origin;
					FVector boxExtend;
					FVector up;
					FVector forward;
					pawn->GetActorBounds(true, origin, boxExtend);
					up = pawn->GetActorUpVector();
					forward = pawn->GetActorForwardVector();
					origin = pawn->GetActorLocation();

					FTaskItemAction tia = tp->GetCurrentTaskItemAction(cur_awake_act_index);
					FVector next_active_action_location = tia.ActLocation;
					FVector dirAndLen = next_active_action_location - origin;
					FVector dir;
					float len;
					dirAndLen.ToDirectionAndLength(dir, len);
					float ha = dir.HeadingAngle();
					ha = FMath::RadiansToDegrees(ha);
					FVector pointer_location = origin + dir * 75.f;

					//
					FHitResult hitResult;
					FCollisionQueryParams collisionQueryParam;
					collisionQueryParam.AddIgnoredActor(pawn);
					GetWorld()->LineTraceSingleByChannel(hitResult, origin, origin - up*boxExtend.Z*2.f, ECollisionChannel::ECC_WorldStatic, collisionQueryParam);
					pointer_location.Z = origin.Z - hitResult.Distance + 50.f;

					FRotator rotate = mesh.mesh->GetActorRotation();
					rotate.Yaw = ha;
					mesh.mesh->SetActorRotation(rotate);
					mesh.mesh->SetActorLocation(pointer_location);
				}
			}
		}
	}
}

void AActorTransformAni::ReachMonitor(float DeltaSeconds)
{
	APawnHandler* ph = FindObject<APawnHandler>(ANY_PACKAGE, TEXT("PawnHandler"));
	if (nullptr == ph)
		return;
	ATaskPool* tp = FindObject<ATaskPool>(ANY_PACKAGE, TEXT("TaskPool"));
	if (nullptr == tp)
		return;
	APawn* pawn = ph->GetCurrentHandlePawn();
	if (nullptr == pawn)
		return;
	FVector pawn_location = pawn->GetActorLocation();
	pawn_location.Z = 0.f;
	for (auto& aspect : TransformAspects)
	{
		if (aspect.Aspect->bMonitorReachedDistance && aspect.Aspect->bRun)
		{
			for (auto& mesh : aspect.Aspect->AspectParams)
			{
				if (mesh.enable )
				{
					mesh.mesh?ActorMeshReachTarget(tp, mesh, pawn_location, DeltaSeconds, aspect.Aspect->AspectName)
						: RangeReachTarget(tp, mesh, pawn_location, aspect.Aspect->AspectName);
				}
				else
				{
					if ( mesh.mesh && !mesh.mesh->IsHidden() )
						mesh.mesh->SetActorHiddenInGame(true);
				}
			}
		}
	}
}

void AActorTransformAni::ActorMeshReachTarget( ATaskPool* tp, FActorAniAspectParam& mesh, FVector& pawn_location, float delta_seconds, FString aspect_name)
{
	if (nullptr == mesh.mesh)
		return;
	UMaterialParameterCollection* pc = LoadObject<UMaterialParameterCollection>(NULL,
		TEXT("/Game/EffectMaterial/TeleportMCP.TeleportMCP"),
		NULL,
		LOAD_None,
		NULL);
	UMaterialParameterCollectionInstance* pci = GetWorld()->GetParameterCollectionInstance(pc);
	FVector location_mesh = mesh.mesh->GetActorLocation();
	FVector size = mesh.mesh->GetActorScale3D();//as 1*1*1 cube.
	location_mesh.Z = 0.f;
	mesh.accumulate_delay_time += delta_seconds;
	float dis = FVector::Dist(location_mesh, pawn_location);
	if (dis <= FMath::Max(size.X * ata_x, size.Y * ata_y))
	{
		//reached.
		if (mesh.accumulate_delay_time <= 1.f)
		{
			//not yet.
			if (pci)
				pci->SetVectorParameterValue(TEXT("Color"), FLinearColor(0.f, 1.f, 0.f, 1.f));
			return;
		}
		mesh.mesh->SetActorHiddenInGame(true);
		mesh.enable = false;
		mesh.accumulate_delay_time = 0.f;
		ActiveMeshPointer(false);

		if (mesh.trigger_callback)
		{
			//notify completed.
			ACallbackActor* ca = FindObject<ACallbackActor>(ANY_PACKAGE, TEXT("CallbackActor"));
			if (nullptr != ca)
			{
				//restore rotating effect.
				if (pci)
					pci->SetVectorParameterValue(TEXT("Color"), FLinearColor(0.f, 0.318547f, 0.814847f, 1.f));

				ca->SetCallbackType(ECallbackType::Callback_NotifyReachedTarget);
				ca->NotifyReachedTarget(
					tp->GetCurrentTaskIndex(),
					tp->GetCurrentTaskItemIndex(),
					tp->GetCurrentTaskItemAwakeActionIndex(),
					aspect_name,
					mesh.mesh->GetName());
			}
		}
		else
		{
			//restore rotating effect.
			if (pci)
				pci->SetVectorParameterValue(TEXT("Color"), FLinearColor(0.f, 0.318547f, 0.814847f, 1.f));
		}
	}
	else
	{
		if (pci)
			pci->SetVectorParameterValue(TEXT("Color"), FLinearColor(0.f, 0.318547f, 0.814847f, 1.f));
		mesh.accumulate_delay_time = 0.f;
	}
}		  

void AActorTransformAni::RangeReachTarget(class ATaskPool* tp, FActorAniAspectParam & mesh, FVector & pawn_location, FString aspect_name)
{
	//consider as range.
	if (nullptr != mesh.mesh)
		return;
	FVector range_center(mesh.area_range);
	range_center.Z = 0.f;
	float dis = FVector::Dist(range_center, pawn_location);
	if (dis <= mesh.area_range.W)
	{
		//reached.
		mesh.enable = false;
		if (mesh.trigger_callback)
		{
			//notify completed.
			ACallbackActor* ca = FindObject<ACallbackActor>(ANY_PACKAGE, TEXT("CallbackActor"));
			if (nullptr != ca)
			{
				ca->SetCallbackType(ECallbackType::Callback_NotifyReachedTarget);
				ca->NotifyReachedTarget(
					tp->GetCurrentTaskIndex(),
					tp->GetCurrentTaskItemIndex(),
					tp->GetCurrentTaskItemAwakeActionIndex(),
					aspect_name,
					TEXT("Ranger"));
			}
		}
		ActiveMeshPointer(false);
	}
}

void AActorTransformAni::ActiveMeshPointer(bool active)
{
	for (auto& aspect : TransformAspects)
	{
		if (aspect.Aspect.Get() && TransformAspect_Pointer == aspect.Aspect->AspectType )
		{
			aspect.Aspect->bRun = active;
			for ( auto& mesh : aspect.Aspect->AspectParams )
			{
				mesh.enable = active;
				if ( mesh.mesh )
					mesh.mesh->SetActorHiddenInGame(!active);
			}
		}
	}
}

bool AActorTransformAni::SetupActorLocation( int actIndex, const FString name )
{
	//get action based on the based.
	ATaskPool* tp = FindObject<ATaskPool>(ANY_PACKAGE, TEXT("TaskPool"));
	if (nullptr == tp)
		return false;
	FTaskItemAction& ia = tp->GetCurrentTaskItemAction(actIndex);
	if (ia.TaskItemIndex > 0 && ia.ActRelateToLocation )
	{
		if (name.Equals(TEXT("AllAspects")))
		{
			for (auto& aspect : TransformAspects)
			{
				if (aspect.Aspect.Get())
				{
					for (auto& mesh : aspect.Aspect->AspectParams)
					{
						if (mesh.auto_location)
						{
							mesh.enable = true;
							if( mesh.mesh )
								mesh.mesh->SetActorLocation(ia.ActLocation);
						}	
					}
						
				}
			}
			return true;
		}
		else
		{
			FActorTransformAspect& aspect = GetActorTransformAspect(name);
			if (!aspect.AspectName.IsEmpty())
			{
				for (auto& mesh : aspect.AspectParams)
				{
					if (mesh.auto_location)
					{
						mesh.enable = true;
						if( mesh.mesh )
							mesh.mesh->SetActorLocation(ia.ActLocation);
					}	
				}
				return true;
			}
		}
	}
	return false;
}

bool AActorTransformAni::SetupAreaRange(int actIndex, float radius, const FString name)
{
	//get action based on the based.
	ATaskPool* tp = FindObject<ATaskPool>(ANY_PACKAGE, TEXT("TaskPool"));
	if (nullptr == tp)
		return false;
	FTaskItemAction& ia = tp->GetCurrentTaskItemAction(actIndex);
	if (ia.TaskItemIndex > 0 && ia.ActRelateToLocation)
	{
		ActiveMeshPointer(true);
		if (name.Equals(TEXT("AllAspects")))
		{
			for (auto& aspect : TransformAspects)
			{
				if (aspect.Aspect.Get() && TransformAspect_Range == aspect.Aspect->AspectType )
				{
					for (auto& ranger : aspect.Aspect->AspectParams)
					{
						if (ranger.auto_location)
						{
							ranger.enable = true;
							ranger.area_range = FVector4(ia.ActLocation,radius);
						}
					}
				}
			}
			return true;
		}
		else
		{
			FActorTransformAspect& aspect = GetActorTransformAspect(name);
			if (!aspect.AspectName.IsEmpty() && TransformAspect_Range == aspect.AspectType)
			{
				for (auto& ranger : aspect.AspectParams)
				{
					if (ranger.auto_location)
					{
						ranger.enable = true;
						ranger.area_range = FVector4(ia.ActLocation, radius);
					}
				}
				return true;
			}
		}
	}
	else
	{
		ActiveMeshPointer(false);
	}
	return false;
}													

bool AActorTransformAni::EnableMonitorReachedAspect(bool enable, const FString name)
{
	if (name.Equals(TEXT("AllAspects")))
	{
		for (auto& aspect : TransformAspects)
		{
			if (aspect.Aspect.Get())
				aspect.Aspect->bMonitorReachedDistance = enable;
		}
		return true;
	}
	else
	{
		FActorTransformAspect& aspect = GetActorTransformAspect(name);
		if (!aspect.AspectName.IsEmpty())
		{
			aspect.bMonitorReachedDistance = enable;
			return true;
		}
	}
	return false;
}

bool AActorTransformAni::AddTransformAspect(const FString& name, EActorTransformAspectType type, float sav, float eav, float interval, bool monitor_distance, FVector dir)
{
	FActorTransformAspect& exist_test = GetActorTransformAspect(name);
	if (exist_test.AspectName.IsEmpty())
	{
		ActorTransformWrapper tw;
		tw.Aspect = MakeShareable(new FActorTransformAspect(name,type,sav,eav,interval, monitor_distance,dir));
		TransformAspects.Add(tw);
		return true;
	}
	return false;//already exist.
}

int AActorTransformAni::RemoveTransformAspect(const FString & name)
{
	const ActorTransformWrapper& wrapper = GetWrapperKey(name);
	return TransformAspects.Remove(wrapper);
}

void AActorTransformAni::Reset()
{
	TransformAspects.Empty();
}

FActorTransformAspect & AActorTransformAni::GetActorTransformAspect(const FString & name)
{
	// TODO: insert return statement here
	const ActorTransformWrapper& wrapper = GetWrapperKey(name);
	return *wrapper.Aspect.Get();
}

bool AActorTransformAni::Begin(const FString name)
{
	if (name.Equals(TEXT("AllAspects")))
	{
		for (auto& aspect : TransformAspects)
		{
			if (aspect.Aspect.Get())
			{
				aspect.Aspect->bRun = true;
				for (auto& mesh : aspect.Aspect->AspectParams)
				{
					if( mesh.mesh )
						mesh.mesh->SetActorHiddenInGame(false);
				}
					
			}
		}
		return true;
	}
	else
	{
		FActorTransformAspect& aspect = GetActorTransformAspect(name);
		if (!aspect.AspectName.IsEmpty())
		{
			aspect.bRun = true;
			for (auto& mesh : aspect.AspectParams)
			{
				if (mesh.mesh)
					mesh.mesh->SetActorHiddenInGame(false);
			}
				
			return true;
		}
	}
	return false;
}

bool AActorTransformAni::Stop(const FString name)
{
	if (name.Equals(TEXT("AllAspects")))
	{
		for (auto& aspect : TransformAspects)
		{
			if (aspect.Aspect.Get())
			{
				aspect.Aspect->bRun = false;
				for (auto& mesh : aspect.Aspect->AspectParams)
					mesh.mesh->SetActorHiddenInGame(true);
			}
		}
		return true;
	}
	else
	{
		FActorTransformAspect& aspect = GetActorTransformAspect(name);
		if (!aspect.AspectName.IsEmpty())
		{
			aspect.bRun = false;
			for (auto& mesh : aspect.AspectParams)
				mesh.mesh->SetActorHiddenInGame(true);
			return true;
		}
	}
	return false;
}

bool AActorTransformAni::Restore(const FString name)
{
	if (name.Equals(TEXT("AllAspects")))
	{
		for (auto& aspect : TransformAspects)
		{
			if (aspect.Aspect.Get())
			{
				aspect.Aspect->bReverse = false;
				aspect.Aspect->CurAniValue = 0.f;
			}
		}
		return true;
	}
	else
	{
		FActorTransformAspect& aspect = GetActorTransformAspect(name);
		if (!aspect.AspectName.IsEmpty())
		{
			aspect.bReverse = false;
			aspect.CurAniValue = 0.f;
			return true;
		}
	}
	return false;
}

bool AActorTransformAni::EnableRun(bool enable, const FString name)
{
	if (name.Equals(TEXT("AllAspects")))
	{
		for (auto& aspect : TransformAspects)
		{
			if (aspect.Aspect.Get())
				aspect.Aspect->bRun = enable;
		}
		return true;
	}
	else
	{
		FActorTransformAspect& aspect = GetActorTransformAspect(name);
		if (!aspect.AspectName.IsEmpty())
		{
			aspect.bRun = enable;
			return true;
		}
	}
	return false;
}

bool AActorTransformAni::SetCycle(bool cycle, const FString name)
{
	if (name.Equals(TEXT("AllAspects")))
	{
		for (auto& aspect : TransformAspects)
		{
			if (aspect.Aspect.Get())
				aspect.Aspect->bCycle = cycle;
		}
		return true;
	}
	else
	{
		FActorTransformAspect& aspect = GetActorTransformAspect(name);
		if (!aspect.AspectName.IsEmpty())
		{
			aspect.bCycle = cycle;
			return true;
		}		
	}
	return false;
}

bool AActorTransformAni::SetInterval(float interval, const FString name)
{
	if (name.Equals(TEXT("AllAspects")))
	{
		for (auto& aspect : TransformAspects)
		{
			if (aspect.Aspect.Get())
				aspect.Aspect->Interval = interval;
		}
		return true;
	}
	else
	{
		FActorTransformAspect& aspect = GetActorTransformAspect(name);
		if (!aspect.AspectName.IsEmpty())
		{
			aspect.Interval = interval;
			return true;
		}
	}
	return false;
}

bool AActorTransformAni::SetStartAniValue(float value, const FString name)
{
	if (name.Equals(TEXT("AllAspects")))
	{
		for (auto& aspect : TransformAspects)
		{
			if (aspect.Aspect.Get())
				aspect.Aspect->StartAniValue = value;
		}
		return true;
	}
	else
	{
		FActorTransformAspect& aspect = GetActorTransformAspect(name);
		if (!aspect.AspectName.IsEmpty())
		{
			aspect.StartAniValue = value;
			return true;
		}
	}
	return false;
}

bool AActorTransformAni::SetEndAniValue(float value, const FString name)
{
	if (name.Equals(TEXT("AllAspects")))
	{
		for (auto& aspect : TransformAspects)
		{
			if (aspect.Aspect.Get())
				aspect.Aspect->EndAniValue = value;
		}
		return true;
	}
	else
	{
		FActorTransformAspect& aspect = GetActorTransformAspect(name);
		if (!aspect.AspectName.IsEmpty())
		{
			aspect.EndAniValue = value;
			return true;
		}
	}
	return false;
}

bool AActorTransformAni::SetAniDirection(FVector dir, const FString name)
{
	if (name.Equals(TEXT("AllAspects")))
	{
		for (auto& aspect : TransformAspects)
		{
			if (aspect.Aspect.Get())
				aspect.Aspect->AniDirection = dir;
		}
		return true;
	}
	else
	{
		FActorTransformAspect& aspect = GetActorTransformAspect(name);
		if (!aspect.AspectName.IsEmpty())
		{
			aspect.AniDirection = dir;
			return true;
		}	
	}
	return false;
}

bool AActorTransformAni::AddAniMesh(const FString name,AStaticMeshActor * mesh, bool enable, bool trigger_callback, bool auto_location)
{
	if (nullptr == mesh)
		return false;
	FActorTransformAspect& aspect = GetActorTransformAspect(name);
	if (!aspect.AspectName.IsEmpty())
	{
		FActorAniAspectParam param;
		param.mesh = mesh;
		param.enable = enable;
		param.trigger_callback = trigger_callback;
		param.auto_location = auto_location;
		aspect.AspectParams.Add(param);
	}
	return true;
}

bool AActorTransformAni::AddAniRange(const FString name, FVector4 range, bool enable, bool trigger_callback, bool auto_location)
{
	FActorTransformAspect& aspect = GetActorTransformAspect(name);
	if (!aspect.AspectName.IsEmpty())
	{
		FActorAniAspectParam param;
		param.enable = enable;
		param.trigger_callback = trigger_callback;
		param.auto_location = auto_location;
		param.area_range = range;
		aspect.AspectParams.Add(param);
	}
	return true;
}

bool AActorTransformAni::RemoveAniMesh(const FString name, AStaticMeshActor * mesh)
{
	if (nullptr == mesh)
		return false;
	FActorTransformAspect& aspect = GetActorTransformAspect(name);
	if (!aspect.AspectName.IsEmpty())
	{
		FActorAniAspectParam param;
		param.mesh = mesh;
		return 1 == aspect.AspectParams.Remove(param);
	}
	return false;
}

bool AActorTransformAni::EnableAniMesh(const FString name, AStaticMeshActor * mesh, bool enable)
{
	if (nullptr == mesh)
		return false;
	FActorTransformAspect& aspect = GetActorTransformAspect(name);
	if (!aspect.AspectName.IsEmpty())
	{
		FActorAniAspectParam param;
		param.mesh = mesh;
		int index = aspect.AspectParams.Find(param);
		if (index >= 0)
		{
			aspect.AspectParams[index].enable = enable;
			aspect.AspectParams[index].mesh->SetActorHiddenInGame(!enable);
			aspect.AspectParams[index].accumulate_delay_time = 0.f;
			return true;
		}
	}
	return false;
}

AStaticMeshActor * AActorTransformAni::GetAniMesh(const FString name, const FString mesh_name)
{
	FActorTransformAspect& aspect = GetActorTransformAspect(name);
	if (!aspect.AspectName.IsEmpty())
	{
		for (auto& mesh : aspect.AspectParams)
		{
			if (mesh.mesh->GetName().Equals(mesh_name))
				return mesh.mesh;
		}
	}
	return nullptr;
}

AStaticMeshActor * AActorTransformAni::GetPointerMesh()
{
	for (auto& aspect : TransformAspects)
	{
		if (aspect.Aspect.Get() && TransformAspect_Pointer == aspect.Aspect->AspectType)
		{
			for (auto& mesh : aspect.Aspect->AspectParams)
			{
				if (mesh.mesh)
					return mesh.mesh;
			}
		}
	}
	return nullptr;
}
