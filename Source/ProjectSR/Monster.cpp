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


// Sets default values
AMonster::AMonster()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	HPBarWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("HPBar"));
	HPBarWidget->SetupAttachment(RootComponent);



}

// Called when the game starts or when spawned
void AMonster::BeginPlay()
{
	Super::BeginPlay();

	if (IsValid(HPBarWidget))
		HPBarWidget->SetVisibility(false);

	
	
}

// Called every frame
void AMonster::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!HPBarWidget->IsVisible() && HPShowElapsedTime > HPShowLifeTime)
		return;

	if (HPShowElapsedTime <= HPShowLifeTime)
	{
		HPShowElapsedTime += DeltaTime;
		HPBarWidget->SetVisibility(true);

		if (UUtilFunctionLibrary::GetStageGameMode()->GetCurrentUserMode() == EUserModeEnum::ENORMAL)
		{
			FRotator CamRot = UUtilFunctionLibrary::GetBasePlayerController()->PlayerCameraManager->GetCameraRotation();
			CamRot.Yaw = 0.f;
			CamRot.Roll = 0.f;
			CamRot.Pitch = -CamRot.Pitch;
			
			HPBarWidget->SetRelativeRotation(CamRot);
		}
	}
	else
		HPBarWidget->SetVisibility(false);

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
		Destroy();
		return;
	}

	SetHPVisible();
}

void AMonster::SetHPVisible()
{
	HPShowElapsedTime = 0.f;

	UUC_HpBar* HPBar = Cast<UUC_HpBar>(HPBarWidget->GetUserWidgetObject());
	if (IsValid(HPBar))
	{
		HPBar->SetHPPercent(CurHP / MaxHP);
	}
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

