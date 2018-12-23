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
	UUtilFunctionLibrary::GetBuildingManager()->FirstSpawn();
	

	UUtilFunctionLibrary::PlayWidgetAnimation(this, TEXT("UIAppear"), false, EUMGSequencePlayMode::Reverse);
}

void UUP_Ingame::OnClick_ButtonConfirm()
{
	
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
	UUtilFunctionLibrary::GetBuildingManager()->CancelSpawn();
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
		FVector SelectedPointOnNav = UUtilFunctionLibrary::GetBuildingManager()->SelectedPointonNavMesh;
		if (UUtilFunctionLibrary::GetBasePlayerController()->ProjectWorldLocationToScreen(SelectedPointOnNav, ScreenPos))
		{
			float viewScale = UWidgetLayoutLibrary::GetViewportScale(SRGAMEINSTANCE(GEngine)->GetWorld());
			const FVector2D viewportSize = FVector2D(GEngine->GameViewport->Viewport->GetSizeXY());
			viewScale = GetDefault<UUserInterfaceSettings>(UUserInterfaceSettings::StaticClass())->GetDPIScaleBasedOnSize(FIntPoint(viewportSize.X, viewportSize.Y));

			variables->SubUIOverlay->SetRenderTranslation(ScreenPos / viewScale);
		}
	}
}
