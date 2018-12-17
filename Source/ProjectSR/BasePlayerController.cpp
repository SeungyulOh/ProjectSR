// Fill out your copyright notice in the Description page of Project Settings.

#include "BasePlayerController.h"
#include "Engine.h"
#include "UtilFunctionLibrary.h"
#include "BaseLevelScriptActor.h"
#include "StageGameMode.h"
#include "ProjectSR.h"
#include "WidgetLayoutLibrary.h"
#include "Engine/UserInterfaceSettings.h"

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


		InputComponent->BindTouch(IE_Pressed, InputHelper, &UInputHelper::CallbackInputTouchBegin);
		InputComponent->BindTouch(IE_Repeat, InputHelper, &UInputHelper::CallbackInputTouchOver);
		InputComponent->BindTouch(IE_Released, InputHelper, &UInputHelper::CallbackInputTouchEnd);
		
		/*Base Input Settings End*/
	}

}


void ABasePlayerController::CreateJoystick()
{
	if (CurrentTouchInterface == nullptr)
	{
		FStringAssetReference TouchInterfaceName(FString("/Game/Blueprints/BaseTouchInterface.BaseTouchInterface"));
		UTouchInterface* ManualTouchInterface = LoadObject<UTouchInterface>(NULL, *TouchInterfaceName.ToString());
		if (IsValid(ManualTouchInterface))
		{
			ActivateTouchInterface(ManualTouchInterface);
		}
	}
}

void ABasePlayerController::RemoveJoystick()
{
	ActivateTouchInterface(nullptr);
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

void UInputHelper::CallbackInputTouchBegin(ETouchIndex::Type TouchIndex, FVector Location)
{
#if WITH_EDITOR
	FString str = TEXT("CallbackInputTouchBegin idx : ") + FString::FromInt((int)TouchIndex) + TEXT(" Location : ") + FString::FromInt(Location.X) + TEXT(" ,") + FString::FromInt(Location.Y) + TEXT(" ,") + FString::FromInt(Location.Z);
	UE_LOG(LogClass, Log, TEXT("%s"), *str);
#endif

	if (UUtilFunctionLibrary::GetStageGameMode()->GetCurrentUserMode() == EUserModeEnum::ETOPVIEW)
	{
		if (CurrentTouchType == ETouchIndex::MAX_TOUCHES)
		{
			CurrentTouchType = TouchIndex;
			StartPos = FVector2D(Location.X, Location.Y);
			CurrentPos = StartPos;
		}
	}
	else if (UUtilFunctionLibrary::GetStageGameMode()->GetCurrentUserMode() >= EUserModeEnum::EBUILDING_IDLE &&
		UUtilFunctionLibrary::GetStageGameMode()->GetCurrentUserMode() <= EUserModeEnum::EBUILDING_END)
	{
		float viewScale = UWidgetLayoutLibrary::GetViewportScale(SRGAMEINSTANCE(GEngine)->GetWorld());
		const FVector2D viewportSize = FVector2D(GEngine->GameViewport->Viewport->GetSizeXY());
		viewScale = GetDefault<UUserInterfaceSettings>(UUserInterfaceSettings::StaticClass())->GetDPIScaleBasedOnSize(FIntPoint(viewportSize.X, viewportSize.Y));

		UUtilFunctionLibrary::GetBuildingManager()->OnClickedWhenBuildingMode.Broadcast(Location / viewScale);
	}
		
}

void UInputHelper::CallbackInputTouchOver(ETouchIndex::Type TouchIndex, FVector Location)
{
#if WITH_EDITOR
	FString str = TEXT("CallbackInputTouchOver idx : ") + FString::FromInt((int)TouchIndex) + TEXT(" Location : ") + FString::FromInt(Location.X) + TEXT(" ,") + FString::FromInt(Location.Y) + TEXT(" ,") + FString::FromInt(Location.Z);
	UE_LOG(LogClass, Log, TEXT("%s"), *str);
#endif

	if (UUtilFunctionLibrary::GetStageGameMode()->GetCurrentUserMode() != EUserModeEnum::ETOPVIEW)
		return;

	if (CurrentTouchType == ETouchIndex::MAX_TOUCHES)
		return;

	
	DirectionVector = FVector2D(Location.X , Location.Y) - CurrentPos;
	UUtilFunctionLibrary::GetBaseLevelScriptActor()->Callback_DynamicCameraMove(DirectionVector);
	CurrentPos = FVector2D(Location.X, Location.Y);
}

void UInputHelper::CallbackInputTouchEnd(ETouchIndex::Type TouchIndex, FVector Location)
{
#if WITH_EDITOR
	FString str = TEXT("CallbackInputTouchEnd idx : ") + FString::FromInt((int)TouchIndex) + TEXT(" Location : ") + FString::FromInt(Location.X) + TEXT(" ,") + FString::FromInt(Location.Y) + TEXT(" ,") + FString::FromInt(Location.Z);
	UE_LOG(LogClass, Log, TEXT("%s"), *str);
#endif
	EUserModeEnum mode = UUtilFunctionLibrary::GetStageGameMode()->GetCurrentUserMode();
	if (mode == EUserModeEnum::ETOPVIEW)
	{
		if (CurrentTouchType != ETouchIndex::MAX_TOUCHES)
		{
			CurrentTouchType = ETouchIndex::MAX_TOUCHES;
		}
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

