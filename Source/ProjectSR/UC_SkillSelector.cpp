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
#include "TableManager.h"
#include "TableInfos.h"
#include "ObservableManager.h"
#include "Observable_StageData.h"


void UUC_SkillSelector::NativeConstruct()
{
	Super::NativeConstruct();

	if (IsValid(Button_Root))
	{
		Button_Root->SkillButton->OnPressed.Clear();
		Button_Root->SkillButton->OnPressed.AddDynamic(this, &UUC_SkillSelector::OnButtonPressed);

		Button_Root->SkillButton->OnReleased.Clear();
		Button_Root->SkillButton->OnReleased.AddDynamic(this, &UUC_SkillSelector::OnButtonReleased);

		/*TempAdded*/
		FResourceCacheInfos* cacheinfo = SRGAMEINSTANCE(this)->TableManager->GetTableInfo<FResourceCacheInfos>(SRGAMEINSTANCE(this)->TableManager->DTResourceCacheTable, TEXT("Sprite_Buidling"));
		if (cacheinfo)
		{
			if (cacheinfo->CacheObject.IsValid())
				Button_Root->SkillImage->Brush.SetResourceObject(cacheinfo->CacheObject.Get());

			Button_Root->type = ESkillButtonType::EBUIDLING;
		}

		cacheinfo = SRGAMEINSTANCE(this)->TableManager->GetTableInfo<FResourceCacheInfos>(SRGAMEINSTANCE(this)->TableManager->DTResourceCacheTable, TEXT("Sprite_Skill1"));
		if (cacheinfo && ButtonArray.IsValidIndex(0))
		{
			if (cacheinfo->CacheObject.IsValid())
				ButtonArray[0]->SkillImage->Brush.SetResourceObject(cacheinfo->CacheObject.Get());

			ButtonArray[0]->type = ESkillButtonType::EACTIVE;
		}
	}

	ObservableStageData = UObservableManager::Get()->GetObservable<UObservable_StageData>();
	if (ObservableStageData.IsValid())
	{
		ObservableStageData->Register(this);
	}
}

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
		if (Button_Root->type == ESkillButtonType::EBUIDLING)
		{
			int32 ReqGold = 300;
			int32 CurrentGold = UUtilFunctionLibrary::GetStageGameMode()->GetCurrentGold();
			if (ReqGold <= CurrentGold)
				UUtilFunctionLibrary::GetMyCharacter()->SetState(ECharacterState::EWAITINGFORBUILDINGSPOT);
		}
		else if (Button_Root->type == ESkillButtonType::EACTIVE)
		{
			int32 InitialGold = UUtilFunctionLibrary::GetInitialGoldEquivalant(1/* UUtilFunctionLibrary::GetStageGameMode()->GetCurrentStage()*/);
			float ReqGold = InitialGold * 0.2f;
			if (ReqGold <= UUtilFunctionLibrary::GetStageGameMode()->GetCurrentGold())
			{
				UUtilFunctionLibrary::GetStageGameMode()->AddGold(-ReqGold);
				UUtilFunctionLibrary::GetMyCharacter()->SetState(ECharacterState::EPLAYACTIVE1SKILLANIM);
			}
		}
	}
	else if (TouchType == EUserTouchType::ETOUCH2SEC)
	{
		if (IsValid(Button_Candidate))
		{
			Button_Root->SkillButton->OnPressed.Clear();
			Button_Root->SkillButton->OnReleased.Clear();
			Button_Root->/*SkillButton->*/SetRenderTranslation(Button_Candidate->RenderTransform.Translation);


			int32 Idx = ButtonArray.Find(Button_Candidate);
			if (Idx != INDEX_NONE)
			{
				ButtonArray[Idx] = Button_Root;

				Button_Root = Button_Candidate;
				Button_Candidate = nullptr;
				FindCandidateButton();
				Button_Root->SkillButton->OnPressed.AddDynamic(this, &UUC_SkillSelector::OnButtonPressed);
				Button_Root->SkillButton->OnReleased.AddDynamic(this, &UUC_SkillSelector::OnButtonReleased);
				Button_Root->/*SkillButton->*/SetRenderTranslation(FVector2D::ZeroVector);
			}
		}

		for (auto& Element : ButtonArray)
			Element->SetVisibility(ESlateVisibility::Collapsed);

		if (IsValid(CircleImage))
			CircleImage->SetVisibility(ESlateVisibility::Collapsed);
	}
	

	TouchType = EUserTouchType::EEND;
	TouchElapsedTime = 0.f;

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

	TArray<UUC_SkillButton*> TempButtonArray = ButtonArray;

	Algo::Sort(TempButtonArray, [TargetLocation](UUC_SkillButton* Source, UUC_SkillButton* Dest) {
		FVector2D SourceLength = Source->RenderTransform.Translation - TargetLocation;
		FVector2D DestLength = Dest->RenderTransform.Translation - TargetLocation;

		return SourceLength.Size() < DestLength.Size();
	});

	UUC_SkillButton* TargetButton = nullptr;
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

void UUC_SkillSelector::Update()
{

}

void UUC_SkillButton::NativeConstruct()
{
	Super::NativeConstruct();

	UUtilFunctionLibrary::GetStageGameMode()->OnGoldChanged.AddUObject(this, &UUC_SkillButton::Callback_GoldChanged);
}

void UUC_SkillButton::Callback_GoldChanged(int32 Prev, int32 Current)
{

}
