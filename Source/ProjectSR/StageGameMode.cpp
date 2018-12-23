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
#include "BaseLevelScriptActor.h"
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

void UBuildingManager::TouchBegin(FVector location)
{
	//check if splinewall is selected by user.
	FVector viewportLoc = location;
	float viewScale = UWidgetLayoutLibrary::GetViewportScale(SRGAMEINSTANCE(GEngine)->GetWorld());
	const FVector2D viewportSize = FVector2D(GEngine->GameViewport->Viewport->GetSizeXY());
	viewScale = GetDefault<UUserInterfaceSettings>(UUserInterfaceSettings::StaticClass())->GetDPIScaleBasedOnSize(FIntPoint(viewportSize.X, viewportSize.Y));
	viewportLoc *= viewScale;

	FVector WorldLocation = FVector::ZeroVector;
	FVector WorldDirection = FVector::ZeroVector;
	bool bSuccess = UUtilFunctionLibrary::GetBasePlayerController()->DeprojectScreenPositionToWorld(viewportLoc.X, viewportLoc.Y, WorldLocation, WorldDirection);
	if (bSuccess)
	{
		TArray<FHitResult> outResult;
		UUtilFunctionLibrary::GetMyWorld()->LineTraceMultiByChannel(outResult, WorldLocation, WorldLocation + WorldDirection * 99999.f, ECollisionChannel::ECC_WorldDynamic);
		for (size_t i = 0; i < outResult.Num(); ++i)
		{
			ASplineWall* SplineWall = Cast<ASplineWall>(outResult[i].GetActor());
			if (SplineWall)
			{
				SelectedPointonNavMesh = outResult[i].ImpactPoint;
				UUtilFunctionLibrary::GetStageGameMode()->SetUserMode(EUserModeEnum::EBUILDING_WALLSELECTING);
				SelectedWalllately = SplineWall;
				return;
			}
		}
	}
	//check end.

	if (UUtilFunctionLibrary::DeprojectViewportPointToNavMesh(FVector2D(location.X, location.Y), SelectedPointonNavMesh))
	{
		if (!WallPoints.Num())
			UUtilFunctionLibrary::GetStageGameMode()->SetUserMode(EUserModeEnum::EBUILDING_ADDSTART);
		else
		{
			WallPoints.Emplace(SelectedPointonNavMesh);
			SpawnedWalllately->Refresh(WallPoints);
			//UUtilFunctionLibrary::GetStageGameMode()->SetUserMode(EUserModeEnum::EBUILDING_ADDING);
		}
	}
	else
	{
		//UUtilFunctionLibrary::GetStageGameMode()->SetUserMode(EUserModeEnum::EBUILDING_IDLE);
	}
}

void UBuildingManager::TouchOver(FVector location)
{
	if (UUtilFunctionLibrary::DeprojectViewportPointToNavMesh(FVector2D(location.X, location.Y), SelectedPointonNavMesh))
	{
		if (!WallPoints.Num())
			return;

		int32 MaxIdx = WallPoints.Num() - 1;
		if (WallPoints.IsValidIndex(MaxIdx))
		{
			WallPoints[MaxIdx] = SelectedPointonNavMesh;
			SpawnedWalllately->Refresh(WallPoints);
			//UUtilFunctionLibrary::GetStageGameMode()->SetUserMode(EUserModeEnum::EBUILDING_ADDING);
		}
	}
}

void UBuildingManager::TouchEnd(FVector location)
{
}

void UBuildingManager::FirstSpawn()
{
	WallPoints.Emplace(SelectedPointonNavMesh);
	SpawnedWalllately = UUtilFunctionLibrary::GetMyWorld()->SpawnActor<ASplineWall>(SelectedPointonNavMesh, FRotator::ZeroRotator);
	WallArray.Emplace(SpawnedWalllately.Get());
}

void UBuildingManager::CancelSpawn()
{
	if (WallPoints.Num())
	{
		WallPoints.Pop();
		SpawnedWalllately->Refresh(UUtilFunctionLibrary::GetBuildingManager()->WallPoints);
		UUtilFunctionLibrary::GetStageGameMode()->SetUserMode(EUserModeEnum::EBUILDING_ADDING);

		int32 MaxNum = WallPoints.Num();
		if (WallPoints.IsValidIndex(MaxNum - 1))
			SelectedPointonNavMesh = WallPoints[MaxNum - 1];
	}
}
