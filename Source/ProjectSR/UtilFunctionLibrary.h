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
	static AActor*						 GetBaseCamera();
	static UWorld*						 GetMyWorld();
	static class ABasePlayerController*		 GetBasePlayerController();
	static class AStageGameMode*			GetStageGameMode();
	static class AStageGameHUD*				GetStageGameHUD();
	static class UEntityRenderComponent*	GetRenderComponent(AActor* TargetActor);
	

	static class ABaseLevelScriptActor*		 GetBaseLevelScriptActor();
	static class UBuildingManager*			 GetBuildingManager();
	static void PlayWidgetAnimation(UUserWidget* widget, FString AnimnName , bool bLoop , EUMGSequencePlayMode::Type type);
	static class UWidgetAnimation* GetWidgetAnimation(UUserWidget* widget, FString AnimnName);
	static bool DeprojectViewportPointToNavMesh(FVector2D viewportLoc, FVector& outLoc);
	static bool isPartialPath();

	static void SearchMonster(TArray<class AMonster*>& monsters, FVector CenterPoint, float Radius);

	/**/
	static int32 GetRequiredGold(float WallLength);
	static int32 GetInitialGoldEquivalant(int32 CurrentStage);

	UFUNCTION(BlueprintCallable)
	static class ABaseCharacter*			GetMyCharacter();

	UFUNCTION(BlueprintCallable)
	static void StageFail();
	
	
};
