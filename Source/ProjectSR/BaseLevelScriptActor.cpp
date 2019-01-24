// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseLevelScriptActor.h"
#include "ProjectSR.h"
#include "Kismet/GameplayStatics.h"
#include "BaseCharacter.h"
#include "UtilFunctionLibrary.h"
#include "StageGameMode.h"
#include "TableManager.h"
#include "TableInfos.h"
#include "MapFunctionLibrary.h"



void ABaseLevelScriptActor::BeginPlay()
{
	Super::BeginPlay();
	StartCameraPostion = DynamicCamera->GetActorLocation();

	UTableManager* TableManager = SRGAMEINSTANCE(this)->TableManager;
	if (TableManager)
	{
		FTableInfos* tableinfo = TableManager->GetTableInfo<FTableInfos>(TableManager->DTObjectTable, TEXT("Bonnie"));
		if (tableinfo)
		{
			UClass* TargetClass = nullptr;
			if (!tableinfo->BlueprintClass.IsValid())
			{
#ifdef WITH_EDITOR
				TargetClass = tableinfo->BlueprintClass.LoadSynchronous();
#endif
			}
			else
			{
				TargetClass = tableinfo->BlueprintClass.Get();
			}

			TArray<AActor*> OutActors;
			UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), OutActors);

			FVector TargetLocation = OutActors[0]->GetActorLocation();
			FNavLocation OutLocation;
			UMapFunctionLibrary::ProjectPointToNavigation(TargetLocation, OutLocation);
			TargetLocation = OutLocation.Location;

			FTransform SpawnTransform;
			SpawnTransform.SetTranslation(TargetLocation);
			ABaseCharacter* BaseCharacter = GetWorld()->SpawnActor<ABaseCharacter>(TargetClass, SpawnTransform);
			UUtilFunctionLibrary::GetStageGameMode()->BaseCharacter = BaseCharacter;

			APlayerCameraManager* PCM = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
			if (PCM)
			{
				FAttachmentTransformRules rule = FAttachmentTransformRules::KeepRelativeTransform;
				rule.LocationRule = EAttachmentRule::KeepRelative;
				rule.RotationRule = EAttachmentRule::KeepWorld;

				BaseCamera->AttachToActor(BaseCharacter, rule);
				PCM->SetViewTarget(BaseCamera);
			}
		}
	}

	
	
}

void ABaseLevelScriptActor::Callback_DynamicCameraMove(FVector2D Direction)
{
	FVector CurrentVector = DynamicCamera->GetActorLocation();

	FVector RightVector = DynamicCamera->GetActorRightVector();
	FVector UpVector = DynamicCamera->GetActorUpVector();


	FVector NewLocation = CurrentVector - RightVector * Direction.X*2;
	NewLocation += UpVector * Direction.Y*2;

	DynamicCamera->SetActorLocation(NewLocation);
}

