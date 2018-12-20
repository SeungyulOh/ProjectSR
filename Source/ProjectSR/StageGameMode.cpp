// Fill out your copyright notice in the Description page of Project Settings.

#include "StageGameMode.h"
#include "ProjectSR.h"
#include "Nexus.h"
#include "TableManager.h"
#include "BaseCharacter.h"
#include "BasePlayerController.h"
#include "MapFunctionLibrary.h"
#include "UP_Ingame.h"
#include "UtilFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "WidgetBlueprintLibrary.h"
#include "Engine/UserInterfaceSettings.h"
#include "WidgetLayoutLibrary.h"
#include "SplineWall.h"

AStageGameMode::AStageGameMode()
{
	PlayerControllerClass = ABasePlayerController::StaticClass();

	BuildingManager = CreateDefaultSubobject<UBuildingManager>(TEXT("BuildingManager"));
}

void AStageGameMode::BeginPlay()
{
	Super::BeginPlay();

	UTableManager* TableManager = SRGAMEINSTANCE(this)->TableManager;
	if (TableManager)
	{
		FTableInfos* tableinfo = TableManager->GetTableInfo<FTableInfos>(TableManager->DTObjectTable, TEXT("Nexus"));
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


			/*Spawn Nexus*/
			TArray<AActor*> OutActors;
			UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), OutActors);

			FVector TargetLocation = OutActors[0]->GetActorLocation();
			FNavLocation OutLocation;
			UMapFunctionLibrary::ProjectPointToNavigation(TargetLocation, OutLocation);
			TargetLocation = OutLocation.Location;

			FTransform SpawnTransform;
			SpawnTransform.SetTranslation(TargetLocation);
			BaseCharacter = GetWorld()->SpawnActor<ABaseCharacter>(TargetClass, SpawnTransform);
		}
	}

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
#ifdef WITH_EDITOR
				WidgetClass = TableInfo->WidgetClass.LoadSynchronous();
#endif
			}

			IngameWidget = CreateWidget<UUP_Ingame>(SRGAMEINSTANCE(GEngine), WidgetClass, WIDGET_INGAME);
			if (IsValid(IngameWidget) && !IngameWidget->IsInViewport())
			{
				IngameWidget->AddToViewport();
			}
		}
	}

	
}

void AStageGameMode::SetUserMode(EUserModeEnum InMode)
{
	if (UserMode != InMode)
	{
		UserMode = InMode;
		DoTasks();
	}

	if (IsValid(IngameWidget))
		IngameWidget->Renderer.Render();
}

void AStageGameMode::DoTasks()
{
	switch (UserMode)
	{
	case EUserModeEnum::ENORMAL:
	{
		APlayerCameraManager* PCM = UGameplayStatics::GetPlayerCameraManager(UUtilFunctionLibrary::GetMyWorld(), 0);
		if (IsValid(PCM))
		{
			if (IsValid(cachedViewtarget))
				PCM->SetViewTarget(cachedViewtarget, SRGAMEINSTANCE(this)->BlendParams);


			//UUtilFunctionLibrary::GetBasePlayerController()->CreateJoystick();
		}
	}break;
	case EUserModeEnum::ETOPVIEW:
	{
		APlayerCameraManager* PCM = UGameplayStatics::GetPlayerCameraManager(UUtilFunctionLibrary::GetMyWorld(), 0);
		if (IsValid(PCM))
		{
			if (!cachedViewtarget)
				cachedViewtarget = PCM->ViewTarget.Target;

			ACameraActor* DynamicCamera = UUtilFunctionLibrary::GetDynamicCamera();
			if (IsValid(DynamicCamera))
				PCM->SetViewTarget(DynamicCamera, SRGAMEINSTANCE(this)->BlendParams);
		}

		//UUtilFunctionLibrary::GetBasePlayerController()->RemoveJoystick();
	}break;
	case EUserModeEnum::EBUILDING_IDLE:
		BuildingManager->WallPoints.Empty();
		break;
	case EUserModeEnum::EBUILDING_ADDSTART:
	{
	}break;
	case EUserModeEnum::EBUILDING_ADDING:
	{
		
	}break;
	case EUserModeEnum::EBUILDING_END:
	{
		//UWidgetBlueprintLibrary::SetInputMode_GameAndUIEx(UUtilFunctionLibrary::GetBasePlayerController());
	}break;

	default:
	{
		
	}break;
	}

	
}
