// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UP_MessageNotifier.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTSR_API UUP_MessageNotifier : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UUP_MessageNotifier")
	class UTextBlock* MessageText;
	
	
	
};
