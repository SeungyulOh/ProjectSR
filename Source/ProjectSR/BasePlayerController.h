// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BasePlayerController.generated.h"

UCLASS()
class PROJECTSR_API UInputHelper : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION()
	void CallbackAxis_MoveUpDown(float AxisValue);
	UFUNCTION()
	void CallbackAxis_MoveLeftRight(float AxisValue);
	UFUNCTION()
	void CallbackAxis_CamUpDown(float AxisValue);
	UFUNCTION()
	void CallbackAxis_CamLeftRight(float AxisValue);
private:
	FVector Calculate_MoveDirectionVector();

private:
	FVector MoveAxis;
};

/**
 * 
 */
UCLASS()
class PROJECTSR_API ABasePlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	ABasePlayerController();

	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

	
	/*Input Helper*/
private:

	UPROPERTY()
	class UInputHelper* InputHelper;
};
