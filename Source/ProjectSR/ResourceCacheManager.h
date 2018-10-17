// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/StreamableManager.h"
#include "ResourceCacheManager.generated.h"


DECLARE_DELEGATE_OneParam(FResourceCacheFinished , uint8)


/**
 * 
 */
UCLASS()
class PROJECTSR_API UResourceCacheManager : public UObject
{
	GENERATED_BODY()

public:
	void Init();

	void Cache_Map();
	void Cache_Remains();

	UFUNCTION()
	void Callback_AsyncLoad();
private:
	void ShowLoadingScene(bool bShow);
	
public:
	FResourceCacheFinished OnResourceCacheFinished;
	
	UPROPERTY()
	class UPackage* CachedMap;

private:
	UPROPERTY()
	TArray<FSoftObjectPath> AssetsQueue;
	UPROPERTY()
	TArray<FSoftObjectPath> CachedAssetPath;
	UPROPERTY()
	TArray<UObject*> CachedResourceArray;
	UPROPERTY()
	TArray<UClass*> CachedUClassArray;
	
	int32 iPartition = 5;

	UPROPERTY()
	class UUP_LoadingScene* LoadingWidget = nullptr;
};
