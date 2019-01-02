// Fill out your copyright notice in the Description page of Project Settings.

#include "UC_SkillSelector.h"
#include "ProjectSR.h"
#include "WidgetBlueprintLibrary.h"
#include "UtilFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "WidgetLayoutLibrary.h"
#include "BaseCharacter.h"
#include "StageGameMode.h"
#include "Engine/UserInterfaceSettings.h"

void UUC_SkillSelector::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (ForceY != 0)
	{
		ForceY = ForceY > 0 ? ForceY - Friction * InDeltaTime : ForceY + Friction * InDeltaTime;
		if (FMath::IsNearlyZero(ForceY) || FMath::Abs(ForceY) < Friction * InDeltaTime)
			ForceY = 0.f;

		Scrolling();
	}


	if (TouchType == EUserTouchType::ESIMPLETOUCH)
	{
		TouchElapsedTime += InDeltaTime;
		if (TouchElapsedTime > TouchStateTransitionTime)
		{
			TouchType = EUserTouchType::ETOUCH2SEC;
			TouchElapsedTime = 0.f;
			Enter_Touch2SecMode();
		}
	}
}

void UUC_SkillSelector::OnButtonPressed()
{
	if(TouchType == EUserTouchType::EEND)
		TouchType = EUserTouchType::ESIMPLETOUCH;

}

void UUC_SkillSelector::OnButtonReleased()
{
	UE_LOG(LogClass, Log, TEXT("Released"));

	/*this means user selected a icon*/
	if (TouchType == EUserTouchType::ESIMPLETOUCH)
	{
		int32 ReqGold = 300;
		int32 CurrentGold = UUtilFunctionLibrary::GetStageGameMode()->GetCurrentGold();
		if(ReqGold <= CurrentGold)
			UUtilFunctionLibrary::GetMyCharacter()->SetState(ECharacterState::EWAITINGFORBUILDINGSPOT);
	}
	else if (TouchType == EUserTouchType::ETOUCH2SEC)
	{
		if (IsValid(Button_Candidate))
		{
			Button_Root->OnPressed.Clear();
			Button_Root->OnReleased.Clear();
			Button_Root->SetRenderTranslation(Button_Candidate->RenderTransform.Translation);

			int32 Idx = ButtonArray.Find(Button_Candidate);
			if (Idx != INDEX_NONE)
			{
				ButtonArray[Idx] = Button_Root;

				Button_Root = Button_Candidate;
				Button_Candidate = nullptr;
				FindCandidateButton();
				Button_Root->OnPressed.AddDynamic(this, &UUC_SkillSelector::OnButtonPressed);
				Button_Root->OnReleased.AddDynamic(this, &UUC_SkillSelector::OnButtonReleased);
				Button_Root->SetRenderTranslation(FVector2D::ZeroVector);
			}
		}

		for (auto& Element : ButtonArray)
			Element->SetVisibility(ESlateVisibility::Collapsed);

		if (IsValid(CircleImage))
			CircleImage->SetVisibility(ESlateVisibility::Collapsed);
	}
	

	TouchType = EUserTouchType::EEND;
	float TouchElapsedTime = 0.f;

}



void UUC_SkillSelector::SetForceY(float inForce)
{
	if (TouchType != EUserTouchType::ETOUCH2SEC)
		return;

	ForceY = FMath::Clamp<float>(inForce, -MaxForce, MaxForce);

	Scrolling();
}

void UUC_SkillSelector::Scrolling()
{
	int32 TotalButtonCount = ButtonArray.Num();
	float StandardAngle = 2 * PI / TotalButtonCount;

	if (!ButtonArray.IsValidIndex(0))
		return;

	FVector2D CurrentPos = ButtonArray[0]->RenderTransform.Translation;
	float angle = 0.f;
	if (CurrentPos.Y >= 0)
		angle = FMath::Acos(CurrentPos.X / Radius);
	else
		angle = 2 * PI - FMath::Acos(CurrentPos.X / Radius);

	for (size_t i = 0; i < ButtonArray.Num(); ++i)
	{
		float OffsetAngle = angle + StandardAngle * i;
		float X = Radius * FMath::Cos(OffsetAngle + ForceY * ForceOffSet);
		float Y = Radius * FMath::Sin(OffsetAngle + ForceY * ForceOffSet);

		ButtonArray[i]->SetRenderTranslation(FVector2D(X, Y));

	}

	FindCandidateButton();
}

void UUC_SkillSelector::FindCandidateButton()
{
	FVector2D TargetLocation = FVector2D(-Radius, 0.f);

	TArray<UButton*> TempButtonArray = ButtonArray;

	Algo::Sort(TempButtonArray, [TargetLocation](UButton* Source, UButton* Dest) {
		FVector2D SourceLength = Source->RenderTransform.Translation - TargetLocation;
		FVector2D DestLength = Dest->RenderTransform.Translation - TargetLocation;

		return SourceLength.Size() < DestLength.Size();
	});

	UButton* TargetButton = nullptr;
	if (TempButtonArray.IsValidIndex(0))
		TargetButton = TempButtonArray[0];

	int32 FoundIdx = ButtonArray.Find(TargetButton);
	if (FoundIdx != INDEX_NONE)
	{
		for (size_t i = 0; i < ButtonArray.Num(); ++i)
		{
			if (i != FoundIdx)
			{
				ButtonArray[i]->SetRenderOpacity(0.3);
			}
			else
			{
				ButtonArray[i]->SetRenderOpacity(1);
				Button_Candidate = ButtonArray[i];
			}
				
		}
	}
}

void UUC_SkillSelector::Enter_Touch2SecMode()
{
	for (auto& Element : ButtonArray)
		Element->SetVisibility(ESlateVisibility::Visible);

	if (IsValid(CircleImage))
		CircleImage->SetVisibility(ESlateVisibility::Visible);

}
