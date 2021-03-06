// Fill out your copyright notice in the Description page of Project Settings.

#include "Monster.h"
#include "ProjectSR.h"
#include "UMG/Public/Components/WidgetComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "TableManager.h"
#include "TableInfos.h"
#include "Kismet/GameplayStatics.h"
#include "UC_HpBar.h"
#include "StageGameMode.h"
#include "BasePlayerController.h"
#include "UtilFunctionLibrary.h"
#include "WidgetLayoutLibrary.h"
#include "Engine/UserInterfaceSettings.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"


// Sets default values
AMonster::AMonster()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AMonster::BeginPlay()
{
	Super::BeginPlay();

	HPBar = CreateWidget<UUC_HpBar>(SRGAMEINSTANCE(GEngine), WidgetClass);

	int32 CurrentStage = 1;// UUtilFunctionLibrary::GetStageGameMode()->GetCurrentStage();
	MaxHP = FMath::Pow(1.2f, CurrentStage) * 20;
	CurHP = MaxHP;
	
}

// Called every frame
void AMonster::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (AbnormalState != EMonsterAbnormalState::IDLE)
	{
		AbnormalElapsedTime += DeltaTime;
		if (AbnormalElapsedTime > AbnormalDuration)
			SetAbnormalState(EMonsterAbnormalState::IDLE, 0.f);
	}

	/*HPBar*/
	if (!HPBar->IsVisible() && HPShowElapsedTime > HPShowLifeTime)
		return;

	if (HPShowElapsedTime <= HPShowLifeTime)
	{
		HPShowElapsedTime += DeltaTime;
		
		if (IsValid(HPBar) && !HPBar->IsInViewport())
			HPBar->AddToViewport();

		HPBar->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

		if (UUtilFunctionLibrary::GetStageGameMode()->GetCurrentUserMode() == EUserModeEnum::ENORMAL)
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
			HPBar->SetRenderTranslation(ScreenPos);
			/*PositionSetting End*/
		}
	}
	else
		HPBar->SetVisibility(ESlateVisibility::Collapsed);


	

}

// Called to bind functionality to input
void AMonster::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AMonster::OnTakeDamage(float Damage)
{
	CurHP -= Damage;
	if (CurHP <= 0)
	{
		if (IsValid(HPBar))
			HPBar->RemoveFromViewport();
		
		Destroy();
		UUtilFunctionLibrary::GetStageGameMode()->DecreaseMonsterCount();
		UUtilFunctionLibrary::GetStageGameMode()->AddGold(MaxHP);
		
		return;
	}

	SetHPVisible();
}

void AMonster::SetHPVisible()
{
	HPShowElapsedTime = 0.f;

	if (IsValid(HPBar))
		HPBar->SetHPPercent(CurHP / MaxHP);
}

void AMonster::SetAbnormalState(EMonsterAbnormalState InState, float Duration)
{
	if (AbnormalState == InState)
		return;

	/*UBlackboardComponent* blackboard = FindComponentByClass<UBlackboardComponent>();
	APawn* pawn = Cast<APawn>(this);
	if (pawn)
	{
		AAIController* AIController = Cast<AAIController>(pawn->GetController());
		if (AIController)
		{
			UBlackboardComponent* blackboard = AIController->GetBlackboardComponent();
			if (IsValid(blackboard))
			{
				blackboard->SetValueAsEnum(TEXT("AbnormalState"), (uint8)InState);
			}
		}
	}*/

	AbnormalState = InState;
	AbnormalDuration = Duration;
	AbnormalElapsedTime = 0.f;
}

FVector AMonster::GetDamageSocketLocation()
{
	USkeletalMeshComponent* SkelComp = Cast<USkeletalMeshComponent>(GetComponentByClass(USkeletalMeshComponent::StaticClass()));
	if (SkelComp)
	{
		return SkelComp->GetSocketLocation(TEXT("Socket_Damage"));
	}

	return FVector::ZeroVector;
}

