// As Part of GuangZhou Training.

#pragma once

UENUM(BlueprintType)
enum EBreathMethod
{
	BREATH_METHOD_LINEAR,
	BREATH_METHOD_EXP
};

class DynamicMaterial
{
public:
	DynamicMaterial();
	~DynamicMaterial();
private:
	//breath class
	class BreathMethod {
	public:
		BreathMethod(const float interval, float peak, EBreathMethod bmt)
			: BreathInterval(interval), BreathPeak(peak), BreathMethodType(bmt)
			, BreathAccTime(0.f), BreathDir(true), BreathFix(false)
		{
			ConstA = FMath::Pow(peak, 2 / interval);
		}
		~BreathMethod(void)
		{
		}
	public:
		EBreathMethod BreathMethodType;
	private:
		float ConstA;
		//limit
		float BreathInterval;
		float BreathPeak;
		//accumulate time and dir
		float BreathAccTime;
		bool BreathDir;
		bool BreathFix;
	public:
		void Breath(float dt, float& strength, float& alpha_strength);
		void SetFixEnergy(float atWhatTime);
		void SetDynamicEnergy();
	};
public:
	struct BreathMesh {
		UMeshComponent* mesh;
		UMaterialInterface* miOrigin;
		UMaterialInstanceDynamic* miReplace;
		int materialIndex;
		FLinearColor emissiveColor;
		FName scalarParamName;
		FName vectorParamName;
		FName texParamName;
		FString meshName;
		bool active;
		BreathMesh() :mesh(nullptr), miOrigin(nullptr), miReplace(nullptr), materialIndex(0), active(false),emissiveColor(1.f, 1.f, 0.f, 1.f) {}
		bool operator < (const BreathMesh& bm) const
		{
			return mesh < bm.mesh && materialIndex < bm.materialIndex;
		}
		bool operator == (const BreathMesh& bm) const
		{
			return mesh == bm.mesh
				&& materialIndex == bm.materialIndex;
		}
	};
private:
	TArray<BreathMesh> BreathMeshes;
	TSharedPtr<BreathMethod> HowToBreath;
public:
	void SetupBreathMethod(float interval, float peak, EBreathMethod type = EBreathMethod::BREATH_METHOD_EXP);
	void Reset();
	bool Begin(const FString& mesh_name, FName tex_param_name,FName scalar_param_name, FName vector_param_name, int material_element, FLinearColor color, UMaterialInterface* mi);
	bool Begin(const FString& mesh_name, UMeshComponent* mesh, FName tex_param_name, FName scalar_param_name, FName vector_param_name, int material_element, FLinearColor color, UMaterialInterface* mi);
	bool BeginWithAllElement(const FString& mesh_name, FName tex_param_name, FName scalar_param_name, FName vector_param_name, FLinearColor color, UMaterialInterface* mi);
	bool End(const FString& mesh_name);//remove from list as well

	bool Active(const FString& mesh_name = TEXT(""));
	bool Restore(const FString& mesh_name = TEXT(""));

	BreathMesh* GetBreathmesh(const FString& mesh_name, const int& material_element);
	bool SetScalarParam(const FString& mesh_name, const int& material_element,const float& v);
	bool SetVectorParam(const FString& mesh_name, const int& material_element, const FLinearColor & v);
	bool SetTextureParam(const FString& mesh_name, const int& material_element, UTexture2D* tex);

	void SetFixEnergy(float atWhatTime);
	void SetDynamicEnergy();
	void Tick(float DeltaSeconds);
};
