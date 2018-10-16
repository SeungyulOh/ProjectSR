// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UP_Start.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTSR_API UUP_Start : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
public:
	UFUNCTION()
	void Callback_ButtonStart();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UP_Start")
	class UButton* Button_Start;
};
