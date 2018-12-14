// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "UserWidget.h"
#include "UtilFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTSR_API UUtilFunctionLibrary : public UObject
{
	GENERATED_BODY()
	
public:
	static ACameraActor*				 GetDynamicCamera();
	static UWorld*						 GetMyWorld();
	static class ABasePlayerController*		 GetBasePlayerController();
	static class AStageGameMode*			GetStageGameMode();
	static class ABaseLevelScriptActor*		 GetBaseLevelScriptActor();
	static class UBuildingManager*			 GetBuildingManager();
	static void PlayWidgetAnimation(UUserWidget* widget, FString AnimnName , bool bLoop , EUMGSequencePlayMode::Type type);
	
	
};
