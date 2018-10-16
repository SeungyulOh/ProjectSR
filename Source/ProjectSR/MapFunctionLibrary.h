// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/Navigation/NavigationTypes.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MapFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTSR_API UMapFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	static bool ProjectPointToNavigation(const FVector& Point, FNavLocation& OutLocation);
	
	
};
