// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseLevelScriptActor.h"
#include "ProjectSR.h"




void ABaseLevelScriptActor::BeginPlay()
{
	Super::BeginPlay();

	StartCameraPostion = DynamicCamera->GetActorLocation();
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
