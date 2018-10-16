// Fill out your copyright notice in the Description page of Project Settings.

#include "MapFunctionLibrary.h"
#include "ProjectSR.h"
#include "NavigationSystem/Public/NavigationSystem.h"


bool UMapFunctionLibrary::ProjectPointToNavigation(const FVector& Point, FNavLocation& OutLocation)
{
	UWorld* world = SRGAMEINSTANCE(GEngine)->GetWorld();
	if (world)
	{
		UNavigationSystemV1* NavigationSystem = Cast<UNavigationSystemV1>(world->GetNavigationSystem());
		if (NavigationSystem)
		{
			return NavigationSystem->ProjectPointToNavigation(Point, OutLocation);
		}
	}

	return false;
}
