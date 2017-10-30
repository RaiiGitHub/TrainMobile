// As Part of GuangZhou Training.

#pragma once

#include "GameFramework/Actor.h"
#include "ActorTransformAni.generated.h"

UENUM(BlueprintType)
enum EActorTransformAspectType
{
	TransformAspect_Location,
	TransformAspect_Scale,
	TransformAspect_Rotation,
	TransformAspect_Range,
	TransformAspect_Pointer
};

USTRUCT(BlueprintType)
struct FActorAniAspectParam {
	GENERATED_USTRUCT_BODY()
public:
	FActorAniAspectParam() :mesh(nullptr), enable(true), trigger_callback(true), auto_location(true), accumulate_delay_time(0.f){}
	bool operator < (const FActorAniAspectParam& ap) const
	{
		return mesh < ap.mesh;
	}
	bool operator == (const FActorAniAspectParam& ap) const
	{
		return mesh == ap.mesh;
	}
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FActorAniAspectParam)
	AStaticMeshActor* mesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FActorAniAspectParam)
	FVector4 area_range;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FActorAniAspectParam)
	bool enable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FActorAniAspectParam)
	bool trigger_callback;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FActorAniAspectParam)
	bool auto_location;//calculate from current active location
public:
	float accumulate_delay_time;
};

USTRUCT(BlueprintType)
struct FActorTransformAspect {
	GENERATED_USTRUCT_BODY()
public:
	FActorTransformAspect() {}
	FActorTransformAspect(const FString& name, EActorTransformAspectType type, float sav, float eav, float interval,bool monitor_distance,FVector dir)
		:AspectName(name), AspectType(type), bMonitorReachedDistance(monitor_distance),bRun(false), bCycle(true), bReverse(false),CurAniValue(0.f),Interval(interval), StartAniValue(sav), EndAniValue(eav), AniDirection(dir)
	{
	}
public:
	float CurAniValue;
	bool bReverse;
	bool bMonitorReachedDistance;
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FActorTransformAspect)
	TEnumAsByte<EActorTransformAspectType> AspectType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FActorTransformAspect)
	FString AspectName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FActorTransformAspect)
	bool bRun;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FActorTransformAspect)
	bool bCycle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FActorTransformAspect)
	float Interval;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FActorTransformAspect)
	float StartAniValue;//could be rotating,scaling or lacting, start value.

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FActorTransformAspect)
	float EndAniValue;//could be rotating,scaling or lacting, end value.

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FActorTransformAspect)
	FVector AniDirection;//specific which direction should be took effect.

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = FActorTransformAspect)
	TArray<FActorAniAspectParam> AspectParams;
};

typedef TSharedPtr<FActorTransformAspect> FActorTransformAspectPtr;
struct ActorTransformWrapper {
public:
	ActorTransformWrapper() {}
public:
	bool operator < (const ActorTransformWrapper& tw) const
	{
		if (Aspect.Get() && tw.Aspect.Get())
			return Aspect->AspectName < tw.Aspect->AspectName;
		return false;
	}
	bool operator == (const ActorTransformWrapper& tw) const
	{
		if (Aspect.Get() && tw.Aspect.Get())
			return Aspect->AspectName == tw.Aspect->AspectName;
		return false;
	}
	FActorTransformAspectPtr Aspect;
};


UCLASS()
class TRAINMOBILEMODEST_API AActorTransformAni : public AActor
{
	GENERATED_BODY()
private:
	TArray<ActorTransformWrapper> TransformAspects;
public:	
	// Sets default values for this actor's properties
	AActorTransformAni();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;
private:
	ActorTransformWrapper& GetWrapperKey(const FString& name);
	void AniLocating(float DeltaSeconds);
	void AniRotating(float DeltaSeconds);
	void AniScaling(float DeltaSeconds);
	void AniPointing(float DeltaSeconds);
	void ReachMonitor(float DeltaSeconds);
private:
	void ActorMeshReachTarget(class ATaskPool* tp,FActorAniAspectParam& mesh,FVector& pawn_location, float delta_seconds,FString aspect_name);
	void RangeReachTarget(class ATaskPool* tp, FActorAniAspectParam& mesh, FVector& pawn_location, FString aspect_name);
	void ActiveMeshPointer(bool active);
public:
	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	bool AddTransformAspect(const FString& name, EActorTransformAspectType type, float sav, float eav, float interval,bool monitor_distance,FVector dir);

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	int RemoveTransformAspect(const FString& name);

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	void Reset();

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	FActorTransformAspect& GetActorTransformAspect(const FString& name);

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	bool Begin(const FString name = TEXT("AllAspects"));

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	bool Stop(const FString name = TEXT("AllAspects"));

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	bool Restore(const FString name = TEXT("AllAspects"));

public:
	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	bool EnableRun(bool enable = true,const FString name = TEXT("AllAspects"));

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	bool SetCycle(bool cycle = true,const FString name = TEXT("AllAspects"));

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	bool SetInterval(float interval,const FString name = TEXT("AllAspects"));

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	bool SetStartAniValue(float value, const FString name = TEXT("AllAspects"));

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	bool SetEndAniValue(float value, const FString name = TEXT("AllAspects"));

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	bool SetAniDirection(FVector dir, const FString name = TEXT("AllAspects"));

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	bool AddAniMesh(const FString name,AStaticMeshActor* mesh,bool enable = true,bool trigger_callback = true,bool auto_location = true);

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	bool AddAniRange(const FString name, FVector4 range, bool enable = true, bool trigger_callback = true, bool auto_location = true);

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	bool RemoveAniMesh(const FString name, AStaticMeshActor* mesh);

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	bool EnableAniMesh(const FString name, AStaticMeshActor* mesh, bool enable = true);

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	AStaticMeshActor* GetAniMesh(const FString name, const FString mesh_name);

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	AStaticMeshActor* GetPointerMesh();

public:
	//moving mesh to the specific target at the begining time, caused by the action changed.
	//base on the current active actions.
	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	bool SetupActorLocation(int actIndex, const FString name = TEXT("AllAspects"));

	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	bool SetupAreaRange(int actIndex,float radius,const FString name = TEXT("AllAspects"));

	//automatically monitor the distance between the current controll-pawn and the ani-target.
	UFUNCTION(BlueprintCallable, Category = "CPP2BP")
	bool EnableMonitorReachedAspect(bool enable,const FString name = TEXT("AllAspects"));

};
