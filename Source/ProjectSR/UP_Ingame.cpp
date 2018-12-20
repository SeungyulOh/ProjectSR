// Fill out your copyright notice in the Description page of Project Settings.

#include "UP_Ingame.h"
#include "ProjectSR.h"
#include "UtilFunctionLibrary.h"
#include "BasePlayerController.h"
#include "BaseLevelScriptActor.h"
#include "Kismet/GameplayStatics.h"
#include "TableManager.h"
#include "TableInfos.h"
#include "MapFunctionLibrary.h"
#include "Nexus.h"
#include "StageGameMode.h"
#include "WidgetLayoutLibrary.h"
#include "Engine/UserInterfaceSettings.h"
#include "SplineWall.h"
#include "UP_Ingame.h"
#include "UC_SkillSelector.h"
#include "NavigationSystem.h"




void UUP_Ingame::NativeConstruct()
{
	Super::NativeConstruct();

	UBuildingManager* BuildingManager = UUtilFunctionLibrary::GetBuildingManager();
	if (IsValid(BuildingManager))
	{
		BuildingManager->OnClickedWhenBuildingMode.Clear();
		BuildingManager->OnClickedWhenBuildingMode.AddUObject(this, &UUP_Ingame::Callback_ClickedWhenBuildingMode);
	}

	Renderer.variables = &Variables;
	Renderer.Prepare();
	Renderer.Render();
}

void UUP_Ingame::NativeDestruct()
{
	Super::NativeDestruct();
}

void UUP_Ingame::OnClick_CameraToggle()
{
	/*Just Setting Viewtarget with blending*/
	bool bCurrentMode = bool(UUtilFunctionLibrary::GetStageGameMode()->GetCurrentUserMode());
	EUserModeEnum CurrentMode = EUserModeEnum(!bCurrentMode);
	UUtilFunctionLibrary::GetStageGameMode()->SetUserMode(CurrentMode);
}

void UUP_Ingame::OnClick_ButtonReady()
{
	/*To Do : Need to cache resource later*/
	UUtilFunctionLibrary::GetStageGameMode()->IngameWidget->Variables.SkillSelector->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	UUtilFunctionLibrary::GetStageGameMode()->SetisMonsterSpawned();
	UUtilFunctionLibrary::GetStageGameMode()->SetUserMode(EUserModeEnum::ENORMAL);
	/**/
}

void UUP_Ingame::OnClick_ButtonBuilding()
{
	if(UUtilFunctionLibrary::GetStageGameMode()->GetCurrentUserMode() >= EUserModeEnum::EBUILDING_IDLE)
		UUtilFunctionLibrary::GetStageGameMode()->SetUserMode(EUserModeEnum::ETOPVIEW);
	else
		UUtilFunctionLibrary::GetStageGameMode()->SetUserMode(EUserModeEnum::EBUILDING_IDLE);
}

void UUP_Ingame::OnClick_ButtonFirstSpawn()
{
	DrawDebugBox(UUtilFunctionLibrary::GetMyWorld(), Variables.SelectedPointonNavMesh, FVector(10.f, 10.f, 10.f), FColor::Cyan, false, 10.f, 0.f, 10.f);
	UUtilFunctionLibrary::GetBuildingManager()->WallPoints.Emplace(Variables.SelectedPointonNavMesh);
	UUtilFunctionLibrary::GetBuildingManager()->SpawnedWalllately = UUtilFunctionLibrary::GetMyWorld()->SpawnActor<ASplineWall>(Variables.SelectedPointonNavMesh, FRotator::ZeroRotator);
	UUtilFunctionLibrary::GetBuildingManager()->WallArray.Emplace(UUtilFunctionLibrary::GetBuildingManager()->SpawnedWalllately.Get());

	UUtilFunctionLibrary::PlayWidgetAnimation(this, TEXT("UIAppear"), false, EUMGSequencePlayMode::Reverse);
}

void UUP_Ingame::OnClick_ButtonConfirm()
{
	/*DrawDebugBox(UUtilFunctionLibrary::GetMyWorld(), Variables.SelectedPointonNavMesh, FVector(10.f, 10.f, 10.f), FColor::Cyan, false, 10.f, 0.f, 10.f);
	UUtilFunctionLibrary::GetBuildingManager()->WallPoints.Emplace(Variables.SelectedPointonNavMesh);
	UUtilFunctionLibrary::GetBuildingManager()->SpawnedWalllately->Refresh(UUtilFunctionLibrary::GetBuildingManager()->WallPoints);
	UUtilFunctionLibrary::PlayWidgetAnimation(this, TEXT("UIAppear"), false, EUMGSequencePlayMode::Reverse);*/
	
}

void UUP_Ingame::OnClick_ButtonRemove()
{
	UBuildingManager* BuildingManager = UUtilFunctionLibrary::GetBuildingManager();
	if (BuildingManager)
	{
		if (BuildingManager->SelectedWalllately.IsValid())
		{
			int32 idx = BuildingManager->WallArray.Find(BuildingManager->SelectedWalllately.Get());
			if (idx != INDEX_NONE)
			{
				BuildingManager->WallArray[idx]->Destroy();
				BuildingManager->WallArray.RemoveAt(idx);
				UUtilFunctionLibrary::GetStageGameMode()->SetUserMode(EUserModeEnum::EBUILDING_IDLE);
			}
		}
	}
}

void UUP_Ingame::OnClick_ButtonComplete()
{
	UUtilFunctionLibrary::GetStageGameMode()->SetUserMode(EUserModeEnum::EBUILDING_IDLE);
}

void UUP_Ingame::OnClick_ButtonPrev()
{
	if (UUtilFunctionLibrary::GetBuildingManager()->WallPoints.Num())
	{
		UUtilFunctionLibrary::GetBuildingManager()->WallPoints.Pop();
		UUtilFunctionLibrary::GetBuildingManager()->SpawnedWalllately->Refresh(UUtilFunctionLibrary::GetBuildingManager()->WallPoints);
		UUtilFunctionLibrary::GetStageGameMode()->SetUserMode(EUserModeEnum::EBUILDING_ADDING);

		int32 MaxNum = UUtilFunctionLibrary::GetBuildingManager()->WallPoints.Num();
		if (UUtilFunctionLibrary::GetBuildingManager()->WallPoints.IsValidIndex(MaxNum - 1))
		{
			Variables.SelectedPointonNavMesh = UUtilFunctionLibrary::GetBuildingManager()->WallPoints[MaxNum - 1];
		}
	}
}

/*Decide Building mode here*/
void UUP_Ingame::Callback_ClickedWhenBuildingMode(FVector ViewportLocation)
{
	//check if splinewall is selected by user.
	FVector viewportLoc = ViewportLocation;
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
				Variables.SelectedPointonNavMesh = outResult[i].ImpactPoint;
				Variables.SubUIOverlay->SetRenderTranslation(FVector2D(ViewportLocation.X, ViewportLocation.Y));
				UUtilFunctionLibrary::GetStageGameMode()->SetUserMode(EUserModeEnum::EBUILDING_WALLSELECTING);
				UUtilFunctionLibrary::GetBuildingManager()->SelectedWalllately = SplineWall;
				return;
			}
		}
	}
	//check end.
	
	if (UUtilFunctionLibrary::DeprojectViewportPointToNavMesh(FVector2D(ViewportLocation.X, ViewportLocation.Y), Variables.SelectedPointonNavMesh))
	{
		Variables.SubUIOverlay->SetRenderTranslation(FVector2D(ViewportLocation.X, ViewportLocation.Y));
		UBuildingManager* buildingManager = UUtilFunctionLibrary::GetBuildingManager();
		if (IsValid(buildingManager))
		{
			if(!buildingManager->WallPoints.Num())
				UUtilFunctionLibrary::GetStageGameMode()->SetUserMode(EUserModeEnum::EBUILDING_ADDSTART);
			else 
			{
				DrawDebugBox(UUtilFunctionLibrary::GetMyWorld(), Variables.SelectedPointonNavMesh, FVector(10.f, 10.f, 10.f), FColor::Cyan, false, 10.f, 0.f, 10.f);
				UUtilFunctionLibrary::GetBuildingManager()->WallPoints.Emplace(Variables.SelectedPointonNavMesh);
				UUtilFunctionLibrary::GetBuildingManager()->SpawnedWalllately->Refresh(UUtilFunctionLibrary::GetBuildingManager()->WallPoints);
				UUtilFunctionLibrary::GetStageGameMode()->SetUserMode(EUserModeEnum::EBUILDING_ADDING);
			}
		}
	}
	else
	{
		//UUtilFunctionLibrary::GetStageGameMode()->SetUserMode(EUserModeEnum::EBUILDING_IDLE);
	}
}

void FRenderer::Prepare()
{

}

void FRenderer::Render()
{
	EUserModeEnum CurrentMode = UUtilFunctionLibrary::GetStageGameMode()->GetCurrentUserMode();
	switch (CurrentMode)
	{
		case EUserModeEnum::ENORMAL:
		{
			if (IsValid(variables->SubUIOverlay))
				variables->SubUIOverlay->SetVisibility(ESlateVisibility::Collapsed);
			if (IsValid(variables->Button_CamMode))
				variables->Button_CamMode->SetVisibility(ESlateVisibility::Visible);
			if (IsValid(variables->BuildingButtonText))
				variables->BuildingButtonText->SetText(FText::FromString(TEXT("Building Mode")));
			if (IsValid(variables->CamButtonText))
				variables->CamButtonText->SetText(FText::FromString(TEXT("Goto Topview")));

			if (IsValid(variables->Button_BuildingMode))
				variables->Button_BuildingMode->SetVisibility(ESlateVisibility::Collapsed);
		}break;
		case EUserModeEnum::ETOPVIEW:
		{
			if (IsValid(variables->SubUIOverlay))
				variables->SubUIOverlay->SetVisibility(ESlateVisibility::Collapsed);
			if (IsValid(variables->Button_CamMode))
				variables->Button_CamMode->SetVisibility(ESlateVisibility::Visible);
			if (IsValid(variables->CamButtonText))
				variables->CamButtonText->SetText(FText::FromString(TEXT("Goto Normalview")));
			if (IsValid(variables->Button_BuildingMode))
			{
				if(!UUtilFunctionLibrary::GetStageGameMode()->GetisMonsterSpawned())
					variables->Button_BuildingMode->SetVisibility(ESlateVisibility::Visible);
				else
					variables->Button_BuildingMode->SetVisibility(ESlateVisibility::Collapsed);

				if (IsValid(variables->BuildingButtonText))
					variables->BuildingButtonText->SetText(FText::FromString(TEXT("Building Mode")));
			}
		}break;
		case EUserModeEnum::EBUILDING_IDLE:
		{
			if (IsValid(variables->SubUIOverlay))
				variables->SubUIOverlay->SetVisibility(ESlateVisibility::Collapsed);
			if (IsValid(variables->Button_CamMode))
				variables->Button_CamMode->SetVisibility(ESlateVisibility::Collapsed);
			if (IsValid(variables->Button_BuildingMode))
				variables->Button_BuildingMode->SetVisibility(ESlateVisibility::Visible);

			if (IsValid(variables->BuildingButtonText))
				variables->BuildingButtonText->SetText(FText::FromString(TEXT("Building Mode End")));

			/*if (variables->SubUIOverlay->RenderTransform.Scale == FVector2D(1.f, 1.f))
				UUtilFunctionLibrary::PlayWidgetAnimation(UUtilFunctionLibrary::GetStageGameMode()->IngameWidget, TEXT("UIAppear"), false, EUMGSequencePlayMode::Reverse);*/
		}break;
		case EUserModeEnum::EBUILDING_ADDSTART:
		{
			if (IsValid(variables->SubUIOverlay))
				variables->SubUIOverlay->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

			if (IsValid(variables->Button_Firstspawn))
				variables->Button_Firstspawn->SetVisibility(ESlateVisibility::Visible);
			if (IsValid(variables->Button_Confirm))
				variables->Button_Confirm->SetVisibility(ESlateVisibility::Collapsed);
			if (IsValid(variables->Button_Remove))
				variables->Button_Remove->SetVisibility(ESlateVisibility::Collapsed);
			if (IsValid(variables->Button_Complete))
				variables->Button_Complete->SetVisibility(ESlateVisibility::Collapsed);
			if (IsValid(variables->Button_Prev))
				variables->Button_Prev->SetVisibility(ESlateVisibility::Collapsed);
			


			UUtilFunctionLibrary::PlayWidgetAnimation(UUtilFunctionLibrary::GetStageGameMode()->IngameWidget, TEXT("UIAppear"), false, EUMGSequencePlayMode::Forward);
			
		}break;

		case EUserModeEnum::EBUILDING_ADDING:
		{
			if (IsValid(variables->SubUIOverlay))
				variables->SubUIOverlay->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

			if (IsValid(variables->Button_Firstspawn))
				variables->Button_Firstspawn->SetVisibility(ESlateVisibility::Collapsed);
			if (IsValid(variables->Button_Confirm))
				variables->Button_Confirm->SetVisibility(ESlateVisibility::Collapsed);
			if (IsValid(variables->Button_Remove))
				variables->Button_Remove->SetVisibility(ESlateVisibility::Collapsed);
			if (IsValid(variables->Button_Complete))
				variables->Button_Complete->SetVisibility(ESlateVisibility::Visible);
			if (IsValid(variables->Button_Prev))
			{
				if(UUtilFunctionLibrary::GetBuildingManager()->WallPoints.Num() > 2)
					variables->Button_Prev->SetVisibility(ESlateVisibility::Visible);
				else
					variables->Button_Prev->SetVisibility(ESlateVisibility::Collapsed);
			}
				


			UUtilFunctionLibrary::PlayWidgetAnimation(UUtilFunctionLibrary::GetStageGameMode()->IngameWidget, TEXT("UIAppear"), false, EUMGSequencePlayMode::Forward);

		}break;

		case EUserModeEnum::EBUILDING_WALLSELECTING:
		{
			if (IsValid(variables->SubUIOverlay))
				variables->SubUIOverlay->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

			if (IsValid(variables->Button_Firstspawn))
				variables->Button_Firstspawn->SetVisibility(ESlateVisibility::Collapsed);
			if (IsValid(variables->Button_Confirm))
				variables->Button_Confirm->SetVisibility(ESlateVisibility::Collapsed);
			if (IsValid(variables->Button_Remove))
				variables->Button_Remove->SetVisibility(ESlateVisibility::Visible);
			if (IsValid(variables->Button_Complete))
				variables->Button_Complete->SetVisibility(ESlateVisibility::Collapsed);
			if (IsValid(variables->Button_Prev))
				variables->Button_Prev->SetVisibility(ESlateVisibility::Collapsed);


			UUtilFunctionLibrary::PlayWidgetAnimation(UUtilFunctionLibrary::GetStageGameMode()->IngameWidget, TEXT("UIAppear"), false, EUMGSequencePlayMode::Forward);

		}break;

		default:
		{
			if (IsValid(variables->SubUIOverlay))
				variables->SubUIOverlay->SetVisibility(ESlateVisibility::Collapsed);
			if (IsValid(variables->Button_CamMode))
				variables->Button_CamMode->SetVisibility(ESlateVisibility::Collapsed);
			if (IsValid(variables->Button_BuildingMode))
				variables->Button_BuildingMode->SetVisibility(ESlateVisibility::Collapsed);
		}break;
	}


	if (IsValid(variables->Button_GameStartMode))
	{
		if (!UUtilFunctionLibrary::GetStageGameMode()->GetisMonsterSpawned())
			variables->Button_GameStartMode->SetVisibility(ESlateVisibility::Visible);
		else
			variables->Button_GameStartMode->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void FRenderer::Reposition_SubUI()
{
	if (IsValid(variables->SubUIOverlay))
	{
		if (!variables->SubUIOverlay->IsVisible())
			return;

		FVector2D ScreenPos = FVector2D::ZeroVector;
		if (UUtilFunctionLibrary::GetBasePlayerController()->ProjectWorldLocationToScreen(variables->SelectedPointonNavMesh , ScreenPos))
		{
			float viewScale = UWidgetLayoutLibrary::GetViewportScale(SRGAMEINSTANCE(GEngine)->GetWorld());
			const FVector2D viewportSize = FVector2D(GEngine->GameViewport->Viewport->GetSizeXY());
			viewScale = GetDefault<UUserInterfaceSettings>(UUserInterfaceSettings::StaticClass())->GetDPIScaleBasedOnSize(FIntPoint(viewportSize.X, viewportSize.Y));

			variables->SubUIOverlay->SetRenderTranslation(ScreenPos / viewScale);
		}
	}
}
