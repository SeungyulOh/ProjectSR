// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UP_LoadingScene.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTSR_API UUP_LoadingScene : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UUP_LoadingScene")
	class UProgressBar* ProgressBar;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UUP_LoadingScene")
	class UTextBlock* ResourceText;
	
};
