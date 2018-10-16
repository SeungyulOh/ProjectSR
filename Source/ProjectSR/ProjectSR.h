// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "UMG.h"
#include "Engine.h"
#include "UnrealEngine.h"
#include "Engine/Classes/Kismet/GameplayStatics.h"

#include "SRGameInstance.h"

extern PROJECTSR_API class USRGameInstance*	SRGameInstance;

#if WITH_EDITOR
#define SRGAMEINSTANCE(p) (USRGameInstance::GetSRGameInstance(p))
#else
#define SRGAMEINSTANCE(p) GRGameInstance
#endif