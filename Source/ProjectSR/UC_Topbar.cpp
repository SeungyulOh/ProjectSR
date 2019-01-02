// Fill out your copyright notice in the Description page of Project Settings.

#include "UC_Topbar.h"
#include "ProjectSR.h"
#include "UtilFunctionLibrary.h"
#include "StageGameMode.h"




void UUC_Topbar::NativeConstruct()
{
	Super::NativeConstruct();

	UUtilFunctionLibrary::GetStageGameMode()->OnMonsterCountChanged.Clear();
	UUtilFunctionLibrary::GetStageGameMode()->OnMonsterCountChanged.AddUObject(this , &UUC_Topbar::Callback_MonsterCountChanged);

	UUtilFunctionLibrary::GetStageGameMode()->OnGoldChanged.Clear();
	UUtilFunctionLibrary::GetStageGameMode()->OnGoldChanged.AddUObject(this, &UUC_Topbar::Callback_GoldChanged);

	UUtilFunctionLibrary::GetStageGameMode()->BuildingManager->OnExpectGoldConsumption.Clear();
	UUtilFunctionLibrary::GetStageGameMode()->BuildingManager->OnExpectGoldConsumption.AddUObject(this, &UUC_Topbar::Callback_ExpectGoldConsumption);

	int32 InitialMonsterCount = UUtilFunctionLibrary::GetStageGameMode()->GetMonsterReaminsCount();
	if (IsValid(Variables.RemainingText))
		Variables.RemainingText->SetText(FText::AsNumber(InitialMonsterCount));
}

void UUC_Topbar::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (ElapsedTime < Duration)
	{
		ElapsedTime += InDeltaTime;
		float Gold = FMath::Lerp<float, float>(PrevGold, FinalGold, ElapsedTime / Duration);

		if(ElapsedTime >= Duration)
			Gold = FinalGold;

		if (IsValid(Variables.GoldText))
			Variables.GoldText->SetText(FText::AsNumber((int32)Gold));
	}

	
}

void UUC_Topbar::Callback_MonsterCountChanged(int32 CurrentMonsterCount)
{
	if (IsValid(Variables.RemainingText))
		Variables.RemainingText->SetText(FText::AsNumber(CurrentMonsterCount));
}

void UUC_Topbar::Callback_GoldChanged(int32 prevgold, int32 finalgold)
{
	PrevGold = prevgold;
	FinalGold = finalgold;
	Duration = 0.25f;
	ElapsedTime = 0.f;
}

void UUC_Topbar::Callback_ExpectGoldConsumption(int32 ReqGold)
{
	if (IsValid(Variables.ExpectedGoldBox))
		Variables.ExpectedGoldBox->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

	if (IsValid(Variables.ExpectedConsumptionText))
		Variables.ExpectedConsumptionText->SetText(FText::AsNumber(ReqGold));
}
