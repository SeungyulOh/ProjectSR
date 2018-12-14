// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/LevelScriptActor.h"
#include "BaseLevelScriptActor.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTSR_API ABaseLevelScriptActor : public ALevelScriptActor
{
	GENERATED_BODY()

public:
	void BeginPlay() override;

	UFUNCTION()
	void Callback_DynamicCameraMove(FVector2D Direction);
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite , Category = "ABaseLevelScriptActor")
	class ACameraActor* DynamicCamera = nullptr;

	FVector StartCameraPostion;
	
	DECLARE_MULTICAST_DELEGATE(FOnMonsterSpawnerActivated)
	FOnMonsterSpawnerActivated OnMonsterSpawnerActivated;
	
};
