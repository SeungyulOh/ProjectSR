// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ResourceCacheManager.generated.h"


DECLARE_DELEGATE(FResourceCacheFinished)

/**
 * 
 */
UCLASS()
class PROJECTSR_API UResourceCacheManager : public UObject
{
	GENERATED_BODY()

public:
	void Cache_Map();
	void Cache_Remains();

public:
	FResourceCacheFinished OnResourceCacheFinished;
	
private:
	/*Map Cache*/
	UPROPERTY()
	TArray<class UPackage*> MapArray;
	/**/
	UPROPERTY()
	TArray<FSoftObjectPath> AssetsQueue;
	UPROPERTY()
	TArray<UObject*> CachedResourceArray;
	UPROPERTY()
	TArray<UClass*> CachedUClassArray;
	
};
