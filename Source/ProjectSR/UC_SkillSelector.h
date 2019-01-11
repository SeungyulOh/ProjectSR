// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SRObserver.h"
#include "UC_SkillSelector.generated.h"

UENUM(BlueprintType)
enum class EUserTouchType : uint8
{
	ESIMPLETOUCH,
	ETOUCH2SEC,
	EEND,
};

UENUM(BlueprintType)
enum class ESkillButtonType : uint8
{
	EBUIDLING,
	EACTIVE,
	EEND,
};


UCLASS()
class PROJECTSR_API UUC_SkillButton : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void Callback_GoldChanged(int32 Prev, int32 Current);

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UUC_SkillButton")
	class UButton* SkillButton;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UUC_SkillButton")
	class UImage* SkillImage;

	int32 CostGoldPercent = 10;

	ESkillButtonType type;
};

/**
 * 
 */
UCLASS()
class PROJECTSR_API UUC_SkillSelector : public UUserWidget , public ISRObserver
{
	GENERATED_BODY()
public:
	void NativeConstruct() override;
	void NativeTick(const FGeometry& MyGeometry, float InDeltaTime);


	UFUNCTION(BlueprintCallable)
	void OnButtonPressed();
	UFUNCTION(BlueprintCallable)
	void OnButtonReleased();

	void SetForceY(float inForce);
	void Scrolling();
	void FindCandidateButton();

	void Enter_Touch2SecMode();

public:
	virtual void Update();
	
public:
	UPROPERTY(EditAnywhere , BlueprintReadWrite , Category = "UUC_SkillSelector")
	TArray<class UUC_SkillButton*> ButtonArray;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UUC_SkillSelector")
	class UUC_SkillButton* Button_Root;
	UPROPERTY()
	class UUC_SkillButton* Button_Candidate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UUC_SkillSelector")
	class UImage* SelectedImage = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UUC_SkillSelector")
	class UImage*	CircleImage = nullptr;

private:
	EUserTouchType TouchType = EUserTouchType::EEND;

	float Radius = 225.f;
	float ForceY = 0.f;
	float Friction = 20.f;
	float MaxForce = 20.f;
	float ForceOffSet = 0.004f;


	float TouchElapsedTime = 0.f;
	float TouchStateTransitionTime = 1.f;

	FIntRect RootButtonPos;

	UPROPERTY()
	TWeakObjectPtr<class UObservable_StageData>	ObservableStageData;
	
	
};
