// Fill out your copyright notice in the Description page of Project Settings.

#include "UC_SkillSelector.h"
#include "ProjectSR.h"
#include "WidgetBlueprintLibrary.h"
#include "UtilFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"




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
	
	
}

void UUC_SkillSelector::OnButtonPressed()
{
	bButtonPressed = true;

	for (auto& Element : ButtonArray)
		Element->SetVisibility(ESlateVisibility::Visible);

	if (IsValid(CircleImage))
		CircleImage->SetVisibility(ESlateVisibility::Visible);
	
}

void UUC_SkillSelector::OnButtonHovered()
{
	if (bButtonPressed)
	{
		UE_LOG(LogClass, Log, TEXT("Dragging"));
	}
}

void UUC_SkillSelector::OnButtonReleased()
{
	bButtonPressed = false;

	UE_LOG(LogClass, Log, TEXT("Released"));

	for (auto& Element : ButtonArray)
		Element->SetVisibility(ESlateVisibility::Collapsed);

	if (IsValid(CircleImage))
		CircleImage->SetVisibility(ESlateVisibility::Collapsed);

}

void UUC_SkillSelector::SetForceY(float inForce)
{
	if (!bButtonPressed)
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

	FindSelectedButton();
}

void UUC_SkillSelector::FindSelectedButton()
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
				ButtonArray[i]->SetRenderOpacity(0.3);
			else
				ButtonArray[i]->SetRenderOpacity(1);
		}
	}
}
