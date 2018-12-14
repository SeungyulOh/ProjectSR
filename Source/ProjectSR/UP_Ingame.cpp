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
	UUtilFunctionLibrary::GetStageGameMode()->SetisMonsterSpawned();
	Renderer.Render();
	/**/
}

void UUP_Ingame::OnClick_ButtonBuilding()
{
	if (UUtilFunctionLibrary::GetStageGameMode()->GetCurrentUserMode() == EUserModeEnum::EBUILDING_START)
		UUtilFunctionLibrary::GetStageGameMode()->SetUserMode(EUserModeEnum::ETOPVIEW);
	else
		UUtilFunctionLibrary::GetStageGameMode()->SetUserMode(EUserModeEnum::EBUILDING_IDLE);
}

void UUP_Ingame::OnClick_ButtonFirstSpawn()
{
	UUtilFunctionLibrary::PlayWidgetAnimation(this, TEXT("UIAppear"), false, EUMGSequencePlayMode::Reverse);
	UUtilFunctionLibrary::GetStageGameMode()->SetUserMode(EUserModeEnum::EBUILDING_START);

	
}

void UUP_Ingame::OnClick_ButtonConfirm()
{
	FVector2D ViewportLocation = Variables.SubUIOverlay->RenderTransform.Translation;
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
				UUtilFunctionLibrary::GetBuildingManager()->WallPoints.Emplace(outResult[i].ImpactPoint);
				UUtilFunctionLibrary::GetBuildingManager()->CurrentWall->Refresh(UUtilFunctionLibrary::GetBuildingManager()->WallPoints);
			}
		}
	}

	UUtilFunctionLibrary::PlayWidgetAnimation(this, TEXT("UIAppear"), false, EUMGSequencePlayMode::Reverse);
	
}

void UUP_Ingame::Callback_ClickedWhenBuildingMode(FVector ViewportLocation)
{
	Variables.SubUIOverlay->SetRenderTranslation(FVector2D(ViewportLocation.X, ViewportLocation.Y));
	UUtilFunctionLibrary::PlayWidgetAnimation(this, TEXT("UIAppear"), false, EUMGSequencePlayMode::Forward);
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

			if (IsValid(variables->Button_BuildingMode))
				variables->Button_BuildingMode->SetVisibility(ESlateVisibility::Collapsed);
		}break;
		case EUserModeEnum::ETOPVIEW:
		{
			if (IsValid(variables->SubUIOverlay))
				variables->SubUIOverlay->SetVisibility(ESlateVisibility::Collapsed);
			if (IsValid(variables->Button_CamMode))
				variables->Button_CamMode->SetVisibility(ESlateVisibility::Visible);
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
				variables->SubUIOverlay->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			if (IsValid(variables->Button_CamMode))
				variables->Button_CamMode->SetVisibility(ESlateVisibility::Visible);
			if (IsValid(variables->Button_BuildingMode))
				variables->Button_BuildingMode->SetVisibility(ESlateVisibility::Visible);

			if (IsValid(variables->BuildingButtonText))
				variables->BuildingButtonText->SetText(FText::FromString(TEXT("Building Mode")));

			if (IsValid(variables->Button_Firstspawn))
				variables->Button_Firstspawn->SetVisibility(ESlateVisibility::Visible);
			if (IsValid(variables->Button_Confirm))
				variables->Button_Confirm->SetVisibility(ESlateVisibility::Collapsed);
		}break;
		case EUserModeEnum::EBUILDING_END:
		case EUserModeEnum::EBUILDING_START:
		{
			if (IsValid(variables->SubUIOverlay))
				variables->SubUIOverlay->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			if (IsValid(variables->Button_CamMode))
				variables->Button_CamMode->SetVisibility(ESlateVisibility::Visible);
			if (IsValid(variables->Button_BuildingMode))
				variables->Button_BuildingMode->SetVisibility(ESlateVisibility::Visible);

			if (IsValid(variables->Button_Firstspawn))
				variables->Button_Firstspawn->SetVisibility(ESlateVisibility::Collapsed);
			if (IsValid(variables->Button_Confirm))
				variables->Button_Confirm->SetVisibility(ESlateVisibility::Visible);

			if (IsValid(variables->BuildingButtonText))
				variables->BuildingButtonText->SetText(FText::FromString(TEXT("Stop building a wall")));
			
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
