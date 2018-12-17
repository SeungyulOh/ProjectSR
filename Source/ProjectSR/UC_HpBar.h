// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UC_HpBar.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTSR_API UUC_HpBar : public UUserWidget
{
	GENERATED_BODY()
public:
	void SetHPPercent(float hp);

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UUC_HpBar")
	class UProgressBar* HPProgressBar;
	
};
