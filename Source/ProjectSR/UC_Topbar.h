// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UC_Topbar.generated.h"

USTRUCT(BlueprintType)
struct PROJECTSR_API FVariables_Topbar
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FVariables_Topbar")
	class UTextBlock*	StageText;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FVariables_Topbar")
	class UTextBlock*	RemainingText;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FVariables_Topbar")
	class UTextBlock*	GoldText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FVariables_Topbar")
	class UHorizontalBox*	ExpectedGoldBox;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FVariables_Topbar")
	class UTextBlock*	ExpectedConsumptionText;
};

/**
 * 
 */
UCLASS()
class PROJECTSR_API UUC_Topbar : public UUserWidget
{
	GENERATED_BODY()
public:
	void NativeConstruct() override;
	void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;


	UFUNCTION()
	void Callback_MonsterCountChanged(int32 CurrentMonsterCount);
	UFUNCTION()
	void Callback_GoldChanged(int32 prevgold, int32 finalgold);
	UFUNCTION()
	void Callback_ExpectGoldConsumption(int32 ReqGold);
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UUC_Topbar")
	FVariables_Topbar Variables;

	/*Gold Related*/
	int32 PrevGold = 0;
	int32 FinalGold = 0;
	float Duration = 0.f;
	float ElapsedTime = 0.f;

	
	
};
