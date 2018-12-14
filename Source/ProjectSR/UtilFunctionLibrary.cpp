// Fill out your copyright notice in the Description page of Project Settings.

#include "UtilFunctionLibrary.h"
#include "Camera/CameraActor.h"
#include "ProjectSR.h"
#include "BaseLevelScriptActor.h"
#include "BasePlayerController.h"
#include "StageGameMode.h"
#include "UserWidget.h"




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
