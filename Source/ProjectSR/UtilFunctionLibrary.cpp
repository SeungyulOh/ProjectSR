// Fill out your copyright notice in the Description page of Project Settings.

#include "UtilFunctionLibrary.h"
#include "Camera/CameraActor.h"
#include "ProjectSR.h"
#include "BaseLevelScriptActor.h"
#include "BasePlayerController.h"
#include "StageGameMode.h"
#include "UserWidget.h"
#include "Engine/UserInterfaceSettings.h"
#include "WidgetLayoutLibrary.h"
#include "NavigationSystem.h"




ACameraActor* UUtilFunctionLibrary::GetDynamicCamera()
{
	ABaseLevelScriptActor* BaseLevelScriptActor = Cast<ABaseLevelScriptActor>(SRGAMEINSTANCE(GEngine)->GetWorld()->GetLevelScriptActor());
	if (IsValid(BaseLevelScriptActor))
		return BaseLevelScriptActor->DynamicCamera;


	return nullptr;
}

UWorld* UUtilFunctionLibrary::GetMyWorld()
{
	if(GEngine)
		return SRGAMEINSTANCE(GEngine)->GetWorld();

	return nullptr;
}

ABasePlayerController* UUtilFunctionLibrary::GetBasePlayerController()
{
	return Cast<ABasePlayerController>(UGameplayStatics::GetPlayerController(SRGAMEINSTANCE(GEngine)->GetWorld() , 0));
}

class AStageGameMode* UUtilFunctionLibrary::GetStageGameMode()
{
	return Cast<AStageGameMode>(UGameplayStatics::GetGameMode(SRGAMEINSTANCE(GEngine)->GetWorld()));
}

class ABaseLevelScriptActor* UUtilFunctionLibrary::GetBaseLevelScriptActor()
{
	UWorld* world = SRGAMEINSTANCE(GEngine)->GetWorld();
	if (IsValid(world))
	{
		return Cast<ABaseLevelScriptActor>(world->GetLevelScriptActor());
	}

	return nullptr;
}

class UBuildingManager* UUtilFunctionLibrary::GetBuildingManager()
{
	AStageGameMode* StageGameMode = Cast<AStageGameMode>(UGameplayStatics::GetGameMode(SRGAMEINSTANCE(GEngine)->GetWorld()));
	if (IsValid(StageGameMode))
		return StageGameMode->BuildingManager;

	return nullptr;
}

void UUtilFunctionLibrary::PlayWidgetAnimation(UUserWidget* widget, FString AnimnName, bool bLoop , EUMGSequencePlayMode::Type type)
{
	UWidgetBlueprintGeneratedClass* WidgetBlueprintClass = Cast<UWidgetBlueprintGeneratedClass>(widget->GetClass());
	if (WidgetBlueprintClass)
	{
		UWidgetAnimation** FoundAnim = WidgetBlueprintClass->Animations.FindByPredicate([AnimnName](UWidgetAnimation* anim) {
			if (IsValid(anim))
			{
				if (anim->GetName().Contains(AnimnName))
					return true;
			}
			return false;
		});

		if (IsValid(*FoundAnim))
			widget->PlayAnimation(*FoundAnim , 0.f, !bLoop , type);
	}
}

bool UUtilFunctionLibrary::DeprojectViewportPointToNavMesh(FVector2D viewportLoc, FVector& outLoc)
{
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
		UUtilFunctionLibrary::GetMyWorld()->LineTraceMultiByChannel(outResult, WorldLocation, WorldLocation + WorldDirection * 10000, ECollisionChannel::ECC_WorldStatic);
		for (size_t i = 0; i < outResult.Num(); ++i)
		{
			ABlockingVolume* BlockingVolume = Cast<ABlockingVolume>(outResult[i].GetActor());
			if (BlockingVolume)
			{
				UNavigationSystemV1* NavSystem = Cast<UNavigationSystemV1>(UUtilFunctionLibrary::GetMyWorld()->GetNavigationSystem());
				if (NavSystem)
				{
					FNavLocation outdata;
					bool bNavSuccess = NavSystem->ProjectPointToNavigation(outResult[i].ImpactPoint, outdata);
					if (bNavSuccess)
					{
						outLoc = outdata.Location;
						return true;
					}
				}
			}
		}
	}

	return false;
}
