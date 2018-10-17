// Fill out your copyright notice in the Description page of Project Settings.

#include "StageGameMode.h"
#include "ProjectSR.h"
#include "Nexus.h"
#include "TableManager.h"
#include "BaseCharacter.h"
#include "BasePlayerController.h"
#include "MapFunctionLibrary.h"
#include "UP_Ingame.h"

AStageGameMode::AStageGameMode()
{
	PlayerControllerClass = ABasePlayerController::StaticClass();
}

void AStageGameMode::BeginPlay()
{
	Super::BeginPlay();

	/*To Do : Need to cache resource later*/
	UTableManager* TableManager = SRGAMEINSTANCE(this)->TableManager;
	if (TableManager)
	{
		FTableInfos* tableinfo = TableManager->GetTableInfo<FTableInfos>(TableManager->DTCharacterTable, TEXT("Nexus"));
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


			/*Spawn Nexus*/
			TArray<AActor*> OutActors;
			UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), OutActors);

			FVector TargetLocation = OutActors[0]->GetActorLocation();
			FNavLocation OutLocation;
			UMapFunctionLibrary::ProjectPointToNavigation(TargetLocation, OutLocation);
			TargetLocation = OutLocation.Location;

			FTransform SpawnTransform;
			SpawnTransform.SetTranslation(TargetLocation);
			GetWorld()->SpawnActor<ANexus>(TargetClass, SpawnTransform);
		}
	}
	/**/


	USRGameInstance* GameInst = SRGAMEINSTANCE(this);
	if (GameInst)
	{
		FWidgetTableInfos* TableInfo = GameInst->TableManager->GetTableInfo<FWidgetTableInfos>(GameInst->TableManager->DTWidgetTable, WIDGET_INGAME);
		if (TableInfo)
		{
			UClass* WidgetClass = nullptr;
			if (TableInfo->WidgetClass.IsValid())
			{
				WidgetClass = TableInfo->WidgetClass.Get();
			}
			else
			{
				WidgetClass = TableInfo->WidgetClass.LoadSynchronous();
			}

			IngameWidget = CreateWidget<UUP_Ingame>(SRGAMEINSTANCE(GEngine), WidgetClass, WIDGET_INGAME);
			if (IsValid(IngameWidget) && !IngameWidget->IsInViewport())
			{
				IngameWidget->AddToViewport();
			}
		}
	}

	
}
