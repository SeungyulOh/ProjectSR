// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "StageGameHUD.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTSR_API AStageGameHUD : public AHUD
{
	GENERATED_BODY()
	

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
	class UUP_Ingame* IngameWidget = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
	class UUP_MessageNotifier*	MessageNotifierWidget = nullptr;
};
