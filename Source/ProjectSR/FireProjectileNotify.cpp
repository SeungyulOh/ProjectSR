// Fill out your copyright notice in the Description page of Project Settings.

#include "FireProjectileNotify.h"
#include "Tower.h"
#include "Monster.h"
#include "TableManager.h"
#include "TableInfos.h"
#include "ProjectSR.h"
#include "UtilFunctionLibrary.h"
#include "Projectile.h"




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
