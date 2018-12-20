// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseLevelScriptActor.h"
#include "ProjectSR.h"
#include "Kismet/GameplayStatics.h"
#include "BaseCharacter.h"
#include "UtilFunctionLibrary.h"




void ABaseLevelScriptActor::BeginPlay()
{
	Super::BeginPlay();

	StartCameraPostion = DynamicCamera->GetActorLocation();

	APlayerCameraManager* PCM = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
	if (PCM)
	{
		FAttachmentTransformRules rule = FAttachmentTransformRules::KeepRelativeTransform;
		rule.LocationRule = EAttachmentRule::KeepRelative;
		rule.RotationRule = EAttachmentRule::KeepWorld;

		BaseCamera->AttachToActor(UUtilFunctionLibrary::GetMyCharacter(), rule);
		PCM->SetViewTarget(BaseCamera);
	}
}

void ABaseLevelScriptActor::Callback_DynamicCameraMove(FVector2D Direction)
{
	FVector CurrentVector = DynamicCamera->GetActorLocation();

	FVector RightVector = DynamicCamera->GetActorRightVector();
	FVector UpVector = DynamicCamera->GetActorUpVector();


	FVector NewLocation = CurrentVector - RightVector * Direction.X*2;
	NewLocation += UpVector * Direction.Y*2;

	DynamicCamera->SetActorLocation(NewLocation);
}
