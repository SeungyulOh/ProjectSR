// Fill out your copyright notice in the Description page of Project Settings.

#include "BasePlayerController.h"
#include "Engine.h"

const float MaxPitch = -20.f;
const float MinPitch = -60.f;


ABasePlayerController::ABasePlayerController()
{
	bShowMouseCursor = true;

	InputHelper = CreateDefaultSubobject<UInputHelper>(TEXT("InputHelper"));
}

void ABasePlayerController::BeginPlay()
{
	Super::BeginPlay();
}

void ABasePlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (InputComponent)
	{
		/*Base Input Settings*/
		InputComponent->BindAxis(FName(TEXT("MoveUpDown")), InputHelper, &UInputHelper::CallbackAxis_MoveUpDown);
		InputComponent->BindAxis(FName(TEXT("MoveLeftRight")), InputHelper, &UInputHelper::CallbackAxis_MoveLeftRight);
		InputComponent->BindAxis(FName(TEXT("CamUpDown")), InputHelper, &UInputHelper::CallbackAxis_CamUpDown);
		InputComponent->BindAxis(FName(TEXT("CamLeftRight")), InputHelper, &UInputHelper::CallbackAxis_CamLeftRight);
		/*Base Input Settings End*/
	}

}


void UInputHelper::CallbackAxis_MoveUpDown(float AxisValue)
{
	if (AxisValue == 0.f)
	{
		MoveAxis.X = AxisValue;
		return;
	}

	
	ABasePlayerController* Parent = Cast<ABasePlayerController>(GetOuter());
	if (IsValid(Parent))
	{
		UCharacterMovementComponent* CharacterMovementComponent = Parent->GetCharacter()->GetCharacterMovement();
		if (IsValid(CharacterMovementComponent))
		{
			float MaxSpeed = CharacterMovementComponent->GetMaxSpeed();
			MoveAxis.X = AxisValue;
			FVector MoveDir = Calculate_MoveDirectionVector();
			CharacterMovementComponent->RequestDirectMove(MoveDir * MaxSpeed, false);
		}
	}
}

void UInputHelper::CallbackAxis_MoveLeftRight(float AxisValue)
{
	if (AxisValue == 0.f)
	{
		MoveAxis.Y = AxisValue;
		return;
	}


	ABasePlayerController* Parent = Cast<ABasePlayerController>(GetOuter());
	if (IsValid(Parent))
	{
		UCharacterMovementComponent* CharacterMovementComponent = Parent->GetCharacter()->GetCharacterMovement();
		if (IsValid(CharacterMovementComponent))
		{
			float MaxSpeed = CharacterMovementComponent->GetMaxSpeed();
			MoveAxis.Y = AxisValue;
			FVector MoveDir = Calculate_MoveDirectionVector();
			CharacterMovementComponent->RequestDirectMove(MoveDir * MaxSpeed, false);
		}
	}
}

void UInputHelper::CallbackAxis_CamUpDown(float AxisValue)
{
	if (AxisValue == 0)
		return;

	ABasePlayerController* Parent = Cast<ABasePlayerController>(GetOuter());
	if (IsValid(Parent))
	{
		USpringArmComponent* SpringArm = Parent->GetCharacter()->FindComponentByClass<USpringArmComponent>();
		FRotator Rot = SpringArm->GetRelativeTransform().Rotator();
		Rot.Pitch += AxisValue;
		Rot.Pitch = FMath::Clamp<float>(Rot.Pitch, MinPitch, MaxPitch);
		SpringArm->SetRelativeRotation(Rot);
	}
}

void UInputHelper::CallbackAxis_CamLeftRight(float AxisValue)
{
	if (AxisValue == 0)
		return;

	ABasePlayerController* Parent = Cast<ABasePlayerController>(GetOuter());
	if (IsValid(Parent))
	{
		USpringArmComponent* SpringArm = Parent->GetCharacter()->FindComponentByClass<USpringArmComponent>();
		FRotator Rot = SpringArm->GetRelativeTransform().Rotator();
		Rot.Yaw += AxisValue;
		SpringArm->SetRelativeRotation(Rot);
	}
}

FVector UInputHelper::Calculate_MoveDirectionVector()
{
	ABasePlayerController* Parent = Cast<ABasePlayerController>(GetOuter());
	if (IsValid(Parent))
	{
		USpringArmComponent* SpringArm = Parent->GetCharacter()->FindComponentByClass<USpringArmComponent>();
		FRotator Rot = SpringArm->GetRelativeTransform().Rotator();

		FVector ForwardVector = Rot.RotateVector(FVector(1.f, 0.f, 0.f));
		FVector RightVector = Rot.RotateVector(FVector(0.f, 1.f, 0.f));

		FVector ResultDir = ForwardVector * MoveAxis.X + RightVector * MoveAxis.Y;
		ResultDir = ResultDir.GetSafeNormal();

		return ResultDir;
	}

	return FVector(1.f, 0.f, 0.f);
}

