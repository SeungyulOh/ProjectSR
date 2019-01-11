// Fill out your copyright notice in the Description page of Project Settings.

#include "FireProjectileNotify.h"
#include "Tower.h"
#include "Monster.h"
#include "TableManager.h"
#include "TableInfos.h"
#include "ProjectSR.h"
#include "UtilFunctionLibrary.h"
#include "Projectile.h"
#include "BaseCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "TowerSpawner.h"




void UFireProjectileNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	ATower* Parent = Cast<ATower>(MeshComp->GetOwner());
	if (IsValid(Parent))
	{
		AMonster* TargetMonster = Parent->GetTargetMonster();
		if (IsValid(TargetMonster))
		{
			UTableManager* TableManager = SRGAMEINSTANCE(GEngine)->TableManager;
			FTableInfos* tableinfo = TableManager->GetTableInfo<FTableInfos>(TableManager->DTObjectTable, TEXT("Arrow"));
			if (tableinfo)
			{
				if (tableinfo->BlueprintClass.IsValid())
				{
					FVector SocketLocation = MeshComp->GetSocketLocation(TEXT("SOCKET_SHOOT"));
					FRotator ParentRot = Parent->GetActorRotation();
					TSubclassOf<AProjectile> ProjectileClass = tableinfo->BlueprintClass.Get();

					FVector ForwardVec = Parent->GetActorForwardVector();
					FVector Dir = TargetMonster->GetDamageSocketLocation() - SocketLocation;
					Dir = Dir.GetSafeNormal2D();

					float cosValue = Dir.DotProduct(Dir, ForwardVec);
					float Angle = FMath::Acos(cosValue);
					ParentRot.Roll = FMath::RadiansToDegrees(Angle);

					FTransform SpawnTransform;
					SpawnTransform.SetLocation(SocketLocation);
					SpawnTransform.SetRotation(ParentRot.Quaternion());

					AProjectile* Projectile = UUtilFunctionLibrary::GetMyWorld()->SpawnActor<AProjectile>(ProjectileClass, SpawnTransform);
					if (IsValid(Projectile))
					{
					}
				}
			}
		}
	}
}

void UTowerBuildingNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	ABaseCharacter* Parent = Cast<ABaseCharacter>(MeshComp->GetOwner());
	if (IsValid(Parent))
	{
		if (Parent->TowerBuildingHelper.HammerHitCount == 0)
		{
			UTableManager* TableManager = SRGAMEINSTANCE(this)->TableManager;
			if (TableManager)
			{
				FTableInfos* tableinfo = TableManager->GetTableInfo<FTableInfos>(TableManager->DTObjectTable, TEXT("TowerSpawner"));
				if (tableinfo)
				{
					if (tableinfo->BlueprintClass.IsValid())
					{
						FVector candidateVector = UUtilFunctionLibrary::GetMyCharacter()->TowerBuildingHelper.CandidateTower->GetActorLocation();
						candidateVector.Z += 100.f;

						TSubclassOf<ATowerSpawner> spawnerclass = tableinfo->BlueprintClass.Get();
						FTransform spawntransform;
						spawntransform.SetLocation(candidateVector);

						ATowerSpawner* towerspawner = UUtilFunctionLibrary::GetMyWorld()->SpawnActor<ATowerSpawner>(spawnerclass , spawntransform);
						towerspawner->TargetTower = UUtilFunctionLibrary::GetMyCharacter()->TowerBuildingHelper.CandidateTower;
						if (IsValid(towerspawner))
							Parent->TowerBuildingHelper.TowerSpawnerArray.Emplace(towerspawner);
					}
				}
			}
		}
		int32 MaxIdx = Parent->TowerBuildingHelper.TowerSpawnerArray.Num() - 1;
		if (Parent->TowerBuildingHelper.TowerSpawnerArray.IsValidIndex(MaxIdx))
		{
			Parent->TowerBuildingHelper.HammerHitCount = Parent->TowerBuildingHelper.HammerHitCount + 1;

			Parent->TowerBuildingHelper.TowerSpawnerArray[MaxIdx]->CurveActiveTime = 0.f;
			if (Parent->TowerBuildingHelper.HammerHitCount == 3)
				Parent->SetState(ECharacterState::ENORMAL);
		}
	}
}

void UReturntoNormalStateNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	ABaseCharacter* Parent = Cast<ABaseCharacter>(MeshComp->GetOwner());
	if (IsValid(Parent))
	{
		Parent->SetState(ECharacterState::ENORMAL);
	}
}

void USkillFireNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	ABaseCharacter* Parent = Cast<ABaseCharacter>(MeshComp->GetOwner());
	if (IsValid(Parent))
	{
		FResourceCacheInfos* tableinfo =SRGAMEINSTANCE(Parent)->TableManager->GetTableInfo<FResourceCacheInfos>(SRGAMEINSTANCE(Parent)->TableManager->DTResourceCacheTable, TEXT("FX_Skill1"));
		if (tableinfo)
		{
			if (tableinfo->CacheObject.IsValid())
			{
				UParticleSystem* ps = Cast<UParticleSystem>(tableinfo->CacheObject.Get());
				if (ps)
				{
					FTransform spawntransform;
					spawntransform.SetLocation(Parent->GetActorLocation());
					spawntransform.SetScale3D(FVector(2.f, 2.f, 1.f));
					UGameplayStatics::SpawnEmitterAtLocation(Parent->GetWorld(),ps, spawntransform);
				}
			}
		}

		TArray<AMonster*> Targets;
		UUtilFunctionLibrary::SearchMonster(Targets, Parent->GetActorLocation(), 600.f);

		for (size_t i = 0; i < Targets.Num(); ++i)
		{
			if (IsValid(Targets[i]))
				Targets[i]->SetAbnormalState(EMonsterAbnormalState::STUNNED, 2.f);
		}
	}
	
}
