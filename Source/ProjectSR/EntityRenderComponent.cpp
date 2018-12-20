// Fill out your copyright notice in the Description page of Project Settings.

#include "EntityRenderComponent.h"
#include "ProjectSR.h"
#include "Materials/MaterialInterface.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Tower.h"


// Sets default values for this component's properties
UEntityRenderComponent::UEntityRenderComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UEntityRenderComponent::BeginPlay()
{
	Super::BeginPlay();

	USkeletalMeshComponent* SkeletalMesh = GetOwner()->FindComponentByClass<USkeletalMeshComponent>();
	if (IsValid(SkeletalMesh))
	{
		TArray<UMaterialInterface*> MatArray;
		SkeletalMesh->GetUsedMaterials(MatArray);

		if (MatArray.IsValidIndex(0))
		{
			UMaterialInstanceConstant* MI = Cast<UMaterialInstanceConstant>(MatArray[0]);
			if (MI)
			{
				OriginalMat = MI;
				DynamicMat = UMaterialInstanceDynamic::Create(MI, this);
			}
		}
	}

	
	// ...
	
}


// Called every frame
void UEntityRenderComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	for (size_t i = 0; i < EffectArray.Num(); )
	{
		if (EffectArray[i].type == FEffectStruct::EType::EAPPEAR)
		{
			if (EffectArray[i].ElapsedTime > EffectArray[i].Duration)
			{
				USkeletalMeshComponent* SkeletalMesh = GetOwner()->FindComponentByClass<USkeletalMeshComponent>();
				if (IsValid(SkeletalMesh))
					SkeletalMesh->SetMaterial(EffectArray[i].MatIndex, OriginalMat);

				EffectArray.RemoveAt(i);

				ATower* Tower = Cast<ATower>(GetOwner());
				if (IsValid(Tower))
					Tower->TowerActivated();

				continue;
			}

			EffectArray[i].ElapsedTime += DeltaTime;
			float Value = (EffectArray[i].Duration - EffectArray[i].ElapsedTime) / EffectArray[i].Duration;
			
			if(IsValid(DynamicMat))
				DynamicMat->SetScalarParameterValue(APPEAR_PARAM, Value);
		}

		++i;
	}

	// ...
}

void UEntityRenderComponent::AddEffectStruct(FEffectStruct effect)
{
	EffectArray.Emplace(effect);

	USkeletalMeshComponent* SkeletalMesh = GetOwner()->FindComponentByClass<USkeletalMeshComponent>();
	if (IsValid(SkeletalMesh))
		SkeletalMesh->SetMaterial(effect.MatIndex, DynamicMat);
}

