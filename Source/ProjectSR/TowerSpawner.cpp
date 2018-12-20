// Fill out your copyright notice in the Description page of Project Settings.

#include "TowerSpawner.h"
#include "ProjectSR.h"
#include "Particles/ParticleSystemComponent.h"
#include "TableManager.h"
#include "TableInfos.h"
#include "BaseCharacter.h"
#include "UtilFunctionLibrary.h"
#include "BasePlayerController.h"
#include "UC_HpBar.h"
#include "WidgetLayoutLibrary.h"
#include "Engine/UserInterfaceSettings.h"
#include "EntityRenderComponent.h"
#include "Tower.h"


// Sets default values
ATowerSpawner::ATowerSpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ParticleSystemComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ParticleSystemComponent"));
	ParticleSystemComponent->SetupAttachment(RootComponent);

}

// Called when the game starts or when spawned
void ATowerSpawner::BeginPlay()
{
	Super::BeginPlay();

	UTableManager* TableManager = SRGAMEINSTANCE(this)->TableManager;
	if (TableManager)
	{
		FResourceCacheInfos* tableinfo = TableManager->GetTableInfo<FResourceCacheInfos>(TableManager->DTResourceCacheTable, TEXT("BuildingCurve"));
		if (tableinfo)
		{
			if (tableinfo->CacheObject.IsValid())
			{
				UCurveFloat* CurveFloat = Cast<UCurveFloat>(tableinfo->CacheObject.Get());
				BuildingCurve = CurveFloat;
			}
		}
	}

	ProgressWidget = CreateWidget<UUC_HpBar>(SRGAMEINSTANCE(GEngine), WidgetClass);
	if (IsValid(ProgressWidget) && !ProgressWidget->IsInViewport())
	{
		ProgressWidget->AddToViewport();
		ProgressWidget->SetVisibility(ESlateVisibility::Collapsed);
	}

	CurrentState = ETowerSpawnerState::ESPAWNING;
}

// Called every frame
void ATowerSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CurrentState == ETowerSpawnerState::ESPAWNING)
	{
		int32 HammerHitCount = UUtilFunctionLibrary::GetMyCharacter()->TowerBuildingHelper.HammerHitCount;
		CurveActiveTime += DeltaTime;
		if (IsValid(BuildingCurve))
		{
			float outfloat = BuildingCurve->GetFloatValue(CurveActiveTime) + (HammerHitCount - 1) * 0.5f;

			FVector CurrentScale = GetActorScale3D();
			CurrentScale.X = outfloat;
			CurrentScale.Y = outfloat;
			CurrentScale.Z = outfloat;

			SetActorScale3D(CurrentScale);
		}

		if (HammerHitCount >= 3 && !ProgressWidget->IsVisible() && CurveActiveTime>0.5f)
		{
			ProgressWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			CurveActiveTime = 0.f;
			CurrentState = ETowerSpawnerState::EWORKING;
		}
	}
	else if (CurrentState == ETowerSpawnerState::EWORKING)
	{
		if (ProgressWidget->IsVisible())
		{
			/*PositionSetting*/
			FVector TargetLocation = GetActorLocation();
			FVector2D ScreenPos = FVector2D::ZeroVector;
			UUtilFunctionLibrary::GetBasePlayerController()->ProjectWorldLocationToScreen(TargetLocation, ScreenPos);

			float viewScale = UWidgetLayoutLibrary::GetViewportScale(SRGAMEINSTANCE(GEngine)->GetWorld());
			const FVector2D viewportSize = FVector2D(GEngine->GameViewport->Viewport->GetSizeXY());
			viewScale = GetDefault<UUserInterfaceSettings>(UUserInterfaceSettings::StaticClass())->GetDPIScaleBasedOnSize(FIntPoint(viewportSize.X, viewportSize.Y));
			ScreenPos /= viewScale;

			ScreenPos.Y -= 20.f;
			ProgressWidget->SetRenderTranslation(ScreenPos);
			/*PositionSetting End*/


			if (CurveActiveTime > Duration)
			{
				ProgressWidget->SetVisibility(ESlateVisibility::Collapsed);
				CurrentState = ETowerSpawnerState::EDISAPPEARING;
			}

			CurveActiveTime += DeltaTime;
			float Percent = CurveActiveTime / Duration;
			Percent = FMath::Clamp<float>(Percent, 0.f, 1.f);

			if (IsValid(ProgressWidget))
				ProgressWidget->HPProgressBar->SetPercent(Percent);
		}
	}
	else if (CurrentState == ETowerSpawnerState::EDISAPPEARING)
	{
		FVector CurrentScale = GetActorScale3D();
		CurrentScale.X -= DeltaTime	* 5.f;
		CurrentScale.Y -= DeltaTime * 5.f;
		CurrentScale.Z -= DeltaTime * 5.f;

		SetActorScale3D(CurrentScale);

		if (CurrentScale.X <= 0)
		{
			CurrentState = ETowerSpawnerState::EEND;
			int32 idx = UUtilFunctionLibrary::GetMyCharacter()->TowerBuildingHelper.TowerSpawnerArray.Find(this);
			if (idx != INDEX_NONE)
				UUtilFunctionLibrary::GetMyCharacter()->TowerBuildingHelper.TowerSpawnerArray.RemoveAt(idx);

			Destroy();

			UEntityRenderComponent* RenderComp = UUtilFunctionLibrary::GetRenderComponent(TargetTower);
			if (IsValid(RenderComp))
			{
				TargetTower->SetActorHiddenInGame(false);

				FEffectStruct effect;
				effect.type = FEffectStruct::EType::EAPPEAR;
				effect.Duration = 1.f;
				RenderComp->AddEffectStruct(effect);
			}
		}
	}

	
		

	

}

