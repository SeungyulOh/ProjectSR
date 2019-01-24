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
#include "GameFramework/PlayerStart.h"
#include "UC_Topbar.h"
#include "SplineWall.h"
#include "UP_MessageNotifier.h"
#include "SlateColor.h"
#include "ObservableManager.h"
#include "StageGameHUD.h"
#include "Observable_StageData.h"

AStageGameMode::AStageGameMode()
{
	PlayerControllerClass = ABasePlayerController::StaticClass();
}

void AStageGameMode::BeginPlay()
{
	Super::BeginPlay();
	TArray<AActor*> OutActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), OutActors);
	if (OutActors.IsValidIndex(0))
		PlayerStartActor = Cast<APlayerStart>(OutActors[0]);

	if (!PlayerStartActor.IsValid())
		return;

	if (!IsValid(BuildingManager))
	{
		BuildingManager = NewObject<UBuildingManager>();
	}


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

	

	/*USRGameInstance* GameInst = SRGAMEINSTANCE(this);
	if (GameInst)
	{
		FWidgetTableInfos* TableInfo = GameInst->TableManager->GetTableInfo<FWidgetTableInfos>(GameInst->TableManager->DTWidgetTable, WIDGET_MESSAGE);
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

			MessageNotifierWidget = (CreateWidget<UUP_MessageNotifier>(GameInst, WidgetClass, WIDGET_MESSAGE));
			if (IsValid(MessageNotifierWidget) && !MessageNotifierWidget->IsInViewport())
			{
				MessageNotifierWidget->AddToViewport();
				MessageNotifierWidget->SetVisibility(ESlateVisibility::Collapsed);
			}
		}
	}

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

			IngameWidget = CreateWidget<UUP_Ingame>(GameInst, WidgetClass, WIDGET_INGAME);
			if (IsValid(IngameWidget) && !IngameWidget->IsInViewport())
			{
				IngameWidget->AddToViewport();
			}
		}
	}*/
	
	UObservable_StageData* StageData = UObservableManager::Get()->GetObservable<UObservable_StageData>();
	if (IsValid(StageData))
	{
		StageData->StageData.CurrentStage = 1;
	}

}

void AStageGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	SpawnerArray.Empty();

	OnGoldChanged.Clear();

	
}

void AStageGameMode::Callback_MessageAnimationEnd()
{
	UUP_Ingame* IngameWidget = UUtilFunctionLibrary::GetStageGameHUD()->IngameWidget;

	switch (GameStateMode)
	{
	case EGameStateEnum::IDLE:
		break;

	case EGameStateEnum::MONSTERSPAWNED:
	{
		if (IsValid(IngameWidget))
			IngameWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}break;

	case EGameStateEnum::STAGEFAILED:
	{
		UGameplayStatics::OpenLevel(SRGAMEINSTANCE(GEngine), TEXT("StartupMap"));
	}break;

	case EGameStateEnum::STAGECLEAR:
	{
		if (IsValid(IngameWidget))
			IngameWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}break;
	}

	
}

void AStageGameMode::SetUserMode(EUserModeEnum InMode)
{
	if (UserMode != InMode)
	{
		UserMode = InMode;
		DoTasks();
	}

	OnUserModeChanged.Broadcast(InMode);

	if (IsValid(IngameWidget))
		IngameWidget->Renderer.Render();
}

void AStageGameMode::SetGameStateMode(EGameStateEnum InMode)
{
	if (GameStateMode != InMode)
	{
		GameStateMode = InMode;

		switch (GameStateMode)
		{
		case EGameStateEnum::IDLE:
			break;

		case EGameStateEnum::MONSTERSPAWNED:
		{
			MessageNotifierWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			UUtilFunctionLibrary::PlayWidgetAnimation(MessageNotifierWidget, TEXT("Stage_Start"), false, EUMGSequencePlayMode::Forward);
			IngameWidget->SetVisibility(ESlateVisibility::Collapsed);

			UWidgetAnimation* anim = UUtilFunctionLibrary::GetWidgetAnimation(MessageNotifierWidget, TEXT("Stage_Start"));
			if (anim)
			{
				anim->OnAnimationFinished.Clear();
				anim->OnAnimationFinished.AddDynamic(this, &AStageGameMode::Callback_MessageAnimationEnd);
			}
		}break;

		case EGameStateEnum::STAGEFAILED:
		{
			MessageNotifierWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			MessageNotifierWidget->MessageText->SetText(FText::FromString(TEXT("StageFailed!")));
			MessageNotifierWidget->MessageText->SetColorAndOpacity(FSlateColor(FLinearColor(0.4f, 0.f, 0.f)));
			UUtilFunctionLibrary::PlayWidgetAnimation(MessageNotifierWidget, TEXT("Message"), false, EUMGSequencePlayMode::Forward);
			IngameWidget->SetVisibility(ESlateVisibility::Collapsed);

			UWidgetAnimation* anim = UUtilFunctionLibrary::GetWidgetAnimation(MessageNotifierWidget, TEXT("Message"));
			if (anim)
			{
				anim->OnAnimationFinished.Clear();
				anim->OnAnimationFinished.AddDynamic(this, &AStageGameMode::Callback_MessageAnimationEnd);
			}

			UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 0.f);
		}break;

		case EGameStateEnum::STAGECLEAR:
		{
			MessageNotifierWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			MessageNotifierWidget->MessageText->SetText(FText::FromString(TEXT("StageFailed!")));
			MessageNotifierWidget->MessageText->SetColorAndOpacity(FSlateColor(FLinearColor(0.4f, 0.f, 0.f)));
			UUtilFunctionLibrary::PlayWidgetAnimation(MessageNotifierWidget, TEXT("Message"), false, EUMGSequencePlayMode::Forward);
			IngameWidget->SetVisibility(ESlateVisibility::Collapsed);

			UWidgetAnimation* anim = UUtilFunctionLibrary::GetWidgetAnimation(MessageNotifierWidget, TEXT("Message"));
			if (anim)
			{
				anim->OnAnimationFinished.Clear();
				anim->OnAnimationFinished.AddDynamic(this, &AStageGameMode::Callback_MessageAnimationEnd);
			}

		}break;

		default:
			break;
		}
	}
}

void AStageGameMode::AddMonsterCount(int32 Count)
{
	MonsterRemains += Count;
	OnMonsterCountChanged.Broadcast(MonsterRemains);
}

void AStageGameMode::DecreaseMonsterCount()
{
	MonsterRemains--;
	OnMonsterCountChanged.Broadcast(MonsterRemains);

	if (MonsterRemains == 0)
	{
		SetGameStateMode(EGameStateEnum::STAGECLEAR);
	}
}

void AStageGameMode::AddGold(int32 MaxHp)
{
	OnGoldChanged.Broadcast(Gold , Gold + MaxHp);
	Gold += MaxHp;
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

	//check if splinewall is selected by user.
	if (UUtilFunctionLibrary::DeprojectViewportPointToNavMesh(FVector2D(location.X, location.Y), SelectedPointonNavMesh))
	{
		if (!WallPoints.Num())
			UUtilFunctionLibrary::GetStageGameMode()->SetUserMode(EUserModeEnum::EBUILDING_ADDSTART);
		else
		{
			UUtilFunctionLibrary::GetStageGameMode()->SetUserMode(EUserModeEnum::EBUILDING_ADDSTART);

			WallPoints.Emplace(SelectedPointonNavMesh);
			SpawnedWalllately->Refresh(WallPoints);

			//need to be fixed.. temp added
			UUtilFunctionLibrary::GetStageGameMode()->IngameWidget->Variables.SubUIOverlay->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
	else
	{
	}
}

void UBuildingManager::TouchOver(FVector location)
{
	if (UUtilFunctionLibrary::GetStageGameMode()->GetCurrentUserMode() == EUserModeEnum::EBUILDING_WALLSELECTING)
		return;

	if (UUtilFunctionLibrary::DeprojectViewportPointToNavMesh(FVector2D(location.X, location.Y), SelectedPointonNavMesh))
	{
		if (!WallPoints.Num())
			return;

		int32 MaxIdx = WallPoints.Num() - 1;
		if (WallPoints.IsValidIndex(MaxIdx))
		{
			WallPoints[MaxIdx] = SelectedPointonNavMesh;
			SpawnedWalllately->Refresh(WallPoints);
		}
	}
}

void UBuildingManager::TouchEnd(FVector location)
{
	if (UUtilFunctionLibrary::GetStageGameMode()->GetCurrentUserMode() == EUserModeEnum::EBUILDING_WALLSELECTING)
		return;

	bool bPartialPath = UUtilFunctionLibrary::isPartialPath();
	if (bPartialPath)
		CancelSpawn();
	else if (WallPoints.Num())
		UUtilFunctionLibrary::GetStageGameMode()->SetUserMode(EUserModeEnum::EBUILDING_ADDING);
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

		int32 MaxNum = WallPoints.Num();
		if (MaxNum <= 1)
			UUtilFunctionLibrary::GetStageGameMode()->SetUserMode(EUserModeEnum::EBUILDING_IDLE);
		else
			UUtilFunctionLibrary::GetStageGameMode()->SetUserMode(EUserModeEnum::EBUILDING_ADDING);

		if (WallPoints.IsValidIndex(MaxNum - 1))
			SelectedPointonNavMesh = WallPoints[MaxNum - 1];
	}
}

void UBuildingManager::OnWallsRefreshed(class ASplineWall* RefreshedWall)
{
	if (!IsValid(RefreshedWall))
		return;

	float TotalLength = RefreshedWall->GetTotalLength();
	ReqGold = UUtilFunctionLibrary::GetRequiredGold(TotalLength);
	OnExpectGoldConsumption.Broadcast(ReqGold);
}
