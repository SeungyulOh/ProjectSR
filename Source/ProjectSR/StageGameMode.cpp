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
	}break;
	case EUserModeEnum::EBUILDING_IDLE:
		break;
	case EUserModeEnum::EBUILDING_START:
	{
		BuildingManager->WallPoints.Empty();

		FVector2D ViewportLocation = IngameWidget->Variables.SubUIOverlay->RenderTransform.Translation;

		float viewScale = UWidgetLayoutLibrary::GetViewportScale(SRGAMEINSTANCE(GEngine)->GetWorld());
		const FVector2D viewportSize = FVector2D(GEngine->GameViewport->Viewport->GetSizeXY());
		viewScale = GetDefault<UUserInterfaceSettings>(UUserInterfaceSettings::StaticClass())->GetDPIScaleBasedOnSize(FIntPoint(viewportSize.X, viewportSize.Y));

		ViewportLocation *= viewScale;

		FVector WorldLocation = FVector::ZeroVector;
		FVector WorldDirection = FVector::ZeroVector;
		bool bSuccess = UUtilFunctionLibrary::GetBasePlayerController()->DeprojectScreenPositionToWorld(ViewportLocation.X, ViewportLocation.Y, WorldLocation, WorldDirection);
		if (bSuccess)
		{
			TArray<FHitResult> outResult;
			UUtilFunctionLibrary::GetMyWorld()->LineTraceMultiByChannel(outResult, WorldLocation, WorldLocation + WorldDirection * 10000, ECollisionChannel::ECC_WorldStatic);
			for (size_t i = 0; i < outResult.Num(); ++i)
			{
				ABlockingVolume* BlockingVolume = Cast<ABlockingVolume>(outResult[i].GetActor());
				if (BlockingVolume)
				{
					DrawDebugBox(UUtilFunctionLibrary::GetMyWorld(), outResult[i].ImpactPoint, FVector(10.f, 10.f, 10.f), FColor::Cyan, false, 10.f, 0.f, 10.f);
					BuildingManager->WallPoints.Emplace(outResult[i].ImpactPoint);

					BuildingManager->CurrentWall = UUtilFunctionLibrary::GetMyWorld()->SpawnActor<ASplineWall>(outResult[i].ImpactPoint , FRotator::ZeroRotator);
					BuildingManager->WallArray.Emplace(BuildingManager->CurrentWall);
					break;
				}
			}
		}
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
