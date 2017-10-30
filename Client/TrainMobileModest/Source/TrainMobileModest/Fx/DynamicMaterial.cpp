#include "TrainMobileModest.h"
#include "DynamicMaterial.h"



DynamicMaterial::DynamicMaterial()
{
}


DynamicMaterial::~DynamicMaterial()
{
}

void DynamicMaterial::BreathMethod::Breath(float dt, float& strength, float& alpha_strength)
{
	//[-interval/2,+interval/2]
	if (EBreathMethod::BREATH_METHOD_EXP == BreathMethodType)
	{
		if (!BreathFix)
		{
			BreathAccTime += BreathDir ? dt : -dt;
			if (BreathAccTime > BreathInterval || BreathAccTime < 0.f)
			{
				BreathDir = !BreathDir;
				BreathAccTime = FMath::Min(BreathInterval, BreathAccTime);
				BreathAccTime = FMath::Max(0.f, BreathAccTime);
			}
		}
		float ba = BreathAccTime - BreathInterval*.5f;
		//final result
		strength = FMath::Pow(ConstA, ba);
		alpha_strength = strength / BreathPeak;
	}
	else if (EBreathMethod::BREATH_METHOD_LINEAR == BreathMethodType)
	{
		if (!BreathFix)
		{
			BreathAccTime += BreathDir ? dt : -dt;
			if (BreathAccTime > BreathInterval || BreathAccTime < 0.f)
			{
				BreathDir = !BreathDir;
				BreathAccTime = FMath::Min(BreathInterval, BreathAccTime);
				BreathAccTime = FMath::Max(0.f, BreathAccTime);
			}
		}
		//final result
		strength = ConstA * BreathAccTime;
		alpha_strength = strength / BreathPeak;
	}
	
}

void DynamicMaterial::BreathMethod::SetFixEnergy(float atWhatTime)
{
	BreathFix = true;
	BreathAccTime = atWhatTime;
}

void DynamicMaterial::BreathMethod::SetDynamicEnergy()
{
	BreathFix = false;
}

void DynamicMaterial::SetupBreathMethod(float interval, float peak,EBreathMethod bmt)
{
	HowToBreath.Reset();
	HowToBreath = MakeShareable(new BreathMethod(interval, peak, bmt));
}

void DynamicMaterial::Reset()
{
	//restore all should be called else where.
	BreathMeshes.Empty();
	HowToBreath.Reset();
}

bool DynamicMaterial::Begin(const FString & mesh_name, FName tex_param_name, FName scalar_param_name, FName vector_param_name, int material_element, FLinearColor color, UMaterialInterface * mi)
{
	AStaticMeshActor* actor = FindObject<AStaticMeshActor>(ANY_PACKAGE, *mesh_name);
	if (nullptr == actor)
		return false;
	return Begin(mesh_name, actor->GetStaticMeshComponent(),tex_param_name,scalar_param_name,vector_param_name,material_element,color,mi);
}

bool DynamicMaterial::Begin(const FString & mesh_name, UMeshComponent * mesh, FName tex_param_name, FName scalar_param_name, FName vector_param_name, int material_element, FLinearColor color, UMaterialInterface * mi)
{
	if (nullptr == mesh)
		return false;
	BreathMesh bm;
	bm.mesh = mesh;
	bm.materialIndex = material_element;
	BreathMesh* p_bm = BreathMeshes.FindByKey(bm);
	if (nullptr == p_bm)
	{
		bm.meshName = mesh_name;
		bm.miOrigin = bm.mesh->GetMaterial(material_element);
		bm.miReplace = bm.mesh->CreateDynamicMaterialInstance(material_element, mi);
		bm.scalarParamName = scalar_param_name;
		bm.vectorParamName = vector_param_name;
		bm.texParamName = tex_param_name;
		bm.emissiveColor = color;
		bm.active = true;
		TArray<UTexture*> texs;
		bm.miOrigin->GetUsedTextures(texs, EMaterialQualityLevel::Medium, true, ERHIFeatureLevel::ES2, true);
		if (texs.Num() > 0)
			bm.miReplace->SetTextureParameterValue(bm.texParamName, texs[0]);
		BreathMeshes.AddUnique(bm);
	}
	else
		p_bm->active = true;
	return true;
}

bool DynamicMaterial::BeginWithAllElement(const FString & mesh_name, FName tex_param_name, FName scalar_param_name, FName vector_param_name, FLinearColor color, UMaterialInterface * mi)
{
	AStaticMeshActor* actor = FindObject<AStaticMeshActor>(ANY_PACKAGE, *mesh_name);
	UMeshComponent* mesh = nullptr;
	if (nullptr == actor)
	{
		ACharacter* charactor = FindObject<ACharacter>(ANY_PACKAGE, *mesh_name);
		if (nullptr == charactor)
			return false;
		mesh = charactor->GetMesh();
	}
	else
	{
		mesh = actor->GetStaticMeshComponent();
	}
	if (nullptr == mesh)
		return false;
	int material_num = mesh->GetNumMaterials();
	for (int material_index = 0; material_index < material_num; ++material_index)
	{
		BreathMesh bm;
		bm.mesh = mesh;
		bm.materialIndex = material_index;
		BreathMesh* p_bm = BreathMeshes.FindByKey(bm);
		if (nullptr == p_bm)
		{
			bm.meshName = mesh_name;
			bm.miOrigin = bm.mesh->GetMaterial(material_index);
			if (bm.miOrigin)
			{
				bm.miReplace = bm.mesh->CreateDynamicMaterialInstance(material_index, mi);
				if (bm.miReplace)
				{
					bm.scalarParamName = scalar_param_name;
					bm.vectorParamName = vector_param_name;
					bm.texParamName = tex_param_name;
					bm.emissiveColor = color;
					bm.active = true;
					TArray<UTexture*> texs;
					bm.miOrigin->GetUsedTextures(texs, EMaterialQualityLevel::Medium, true, ERHIFeatureLevel::ES2, true);
					if (texs.Num() > 0)
						bm.miReplace->SetTextureParameterValue(bm.texParamName, texs[0]);
					BreathMeshes.AddUnique(bm);
				}
			}
		}
		else
			p_bm->active = true;
	}
	return true;
}

bool DynamicMaterial::End(const FString & mesh_name)
{
	//remove from list as well.
	//may be multi.
	bool match = false;
	for (int index = 0; index < BreathMeshes.Num(); ++index)
	{
		if (BreathMeshes[index].meshName.Equals(mesh_name))
		{
			BreathMeshes[index].mesh->SetMaterial(BreathMeshes[index].materialIndex, BreathMeshes[index].miOrigin);
			BreathMeshes.RemoveAt(index);
			match = true;
			--index;
		}
	}
	return match;
}

bool DynamicMaterial::Active(const FString & mesh_name)
{
	if (mesh_name.IsEmpty())
	{
		for (auto& bm : BreathMeshes)
		{
			bm.active = true;
			bm.mesh->SetMaterial(bm.materialIndex, bm.miReplace);
		}
		return true;
	}
	else
	{
		//may more than one.
		bool match = false;
		for (int index = 0; index < BreathMeshes.Num(); ++index)
		{
			if (BreathMeshes[index].meshName.Equals(mesh_name))
			{
				BreathMeshes[index].mesh->SetMaterial(BreathMeshes[index].materialIndex, BreathMeshes[index].miReplace);
				match = true;
				--index;
			}
		}
		return match;
	}
	return false;
}

bool DynamicMaterial::Restore(const FString & mesh_name)
{
	if (mesh_name.IsEmpty())
	{
		for (auto& bm : BreathMeshes)
		{
			bm.active = false;
			bm.mesh->SetMaterial(bm.materialIndex, bm.miOrigin);
		}
	}
	else
	{
		//may more than one.
		bool match = false;
		for (int index = 0; index < BreathMeshes.Num(); ++index)
		{
			if (BreathMeshes[index].meshName.Equals(mesh_name))
			{
				BreathMeshes[index].mesh->SetMaterial(BreathMeshes[index].materialIndex, BreathMeshes[index].miOrigin);
				match = true;
				--index;
			}
		}
		return match;
	}
	return false;
}

DynamicMaterial::BreathMesh* DynamicMaterial::GetBreathmesh(const FString & mesh_name, const int & material_element)
{
	// TODO: insert return statement here
	for (auto& bm : BreathMeshes)
	{
		if (bm.meshName.Equals(mesh_name) && material_element == bm.materialIndex)
			return &bm;
	}
	return nullptr;
}

bool DynamicMaterial::SetScalarParam(const FString & mesh_name, const int & material_element, const float & v)
{
	BreathMesh* bm = GetBreathmesh(mesh_name, material_element);
	if (nullptr == bm)
		return false;
	bm->miReplace->SetScalarParameterValue(bm->scalarParamName, v);
	return true;
}

bool DynamicMaterial::SetVectorParam(const FString & mesh_name, const int & material_element, const FLinearColor & v)
{
	BreathMesh* bm = GetBreathmesh(mesh_name, material_element);
	if (nullptr == bm)
		return false;
	bm->miReplace->SetVectorParameterValue(bm->vectorParamName, v);
	return true;
}

bool DynamicMaterial::SetTextureParam(const FString & mesh_name, const int & material_element, UTexture2D * tex)
{
	BreathMesh* bm = GetBreathmesh(mesh_name, material_element);
	if (nullptr == bm || nullptr == tex)
		return false;
	bm->miReplace->SetTextureParameterValue(bm->texParamName,tex);
	return true;
}

void DynamicMaterial::SetFixEnergy(float atWhatTime)
{
	if (HowToBreath.Get())
	{
		HowToBreath->SetFixEnergy(atWhatTime);
	}
}

void DynamicMaterial::SetDynamicEnergy()
{
	if (HowToBreath.Get())
	{
		HowToBreath->SetDynamicEnergy();
	}
}

void DynamicMaterial::Tick(float DeltaTime)
{
	if (HowToBreath.Get())
	{
		float strength = 0.f;
		float alpha_strength = 0.f;
		HowToBreath->Breath(DeltaTime, strength, alpha_strength);
		int bmsize = BreathMeshes.Num();
		for (auto& bm : BreathMeshes)
		{
			if (bm.active)
			{
				FLinearColor color = bm.emissiveColor;
				color.A *= alpha_strength;
				bm.miReplace->SetScalarParameterValue(bm.scalarParamName, strength);
				bm.miReplace->SetVectorParameterValue(bm.vectorParamName, color);
			}
		}
	}
}
