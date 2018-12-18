// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UC_SkillSelector.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTSR_API UUC_SkillSelector : public UUserWidget
{
	GENERATED_BODY()
public:
	void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);


	UFUNCTION(BlueprintCallable)
	void OnButtonPressed();
	UFUNCTION(BlueprintCallable)
	void OnButtonHovered();
	UFUNCTION(BlueprintCallable)
	void OnButtonReleased();
	

	void SetForceY(float inForce);
	void Scrolling();
	void FindSelectedButton();
	
public:
	UPROPERTY(EditAnywhere , BlueprintReadWrite , Category = "UUC_SkillSelector")
	TArray<class UButton*> ButtonArray;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UUC_SkillSelector")
	class UImage*	CircleImage = nullptr;

private:
	bool bButtonPressed = false;
	float Radius = 225.f;

	float ForceY = 0.f;
	float Friction = 20.f;
	float MaxForce = 20.f;
	float ForceOffSet = 0.004f;
	
	
};
