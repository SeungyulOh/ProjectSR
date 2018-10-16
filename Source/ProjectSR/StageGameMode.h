// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectSRGameModeBase.h"
#include "StageGameMode.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTSR_API AStageGameMode : public AProjectSRGameModeBase
{
	GENERATED_BODY()
	
public:
	AStageGameMode();

	virtual void BeginPlay() override;
	
	
};
