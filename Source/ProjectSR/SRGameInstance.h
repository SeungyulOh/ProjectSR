// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "UObjectGlobals.h"
#include "SRGameInstance.generated.h"

/*
	Manage from UserInput to GameStart

	1. UserInput
	2. Cache Resources(Async)
		2.1. Map
		2.2. Character, Animation, Particle , etc
	3. StartGame
*/
UCLASS()
class PROJECTSR_API UGameStartHelper : public UObject
{
	GENERATED_BODY()
public:
	enum class EPrerequisiteGameStart : uint8
	{
		MAPCACHE,
		REMAINCACHE,
		END
	};

public:
	void Init();
	void Decide_NextAction();

	UFUNCTION()
	void Callback_ResourceCacheFinished(uint8 Sequence);

private:
	void Do_CacheMap();
	void Do_CacheRemains();
	void Do_StartGame();

public:
	int32 CurrentStage = 1;
	TBitArray<FDefaultBitArrayAllocator> RequirementArray;
};

/**
 * 
 */
UCLASS()
class PROJECTSR_API USRGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	USRGameInstance(const FObjectInitializer& ObjectInitializer);
	static class USRGameInstance* GetSRGameInstance(class UObject* OutterOwner);

public:
	/*F.1. Overrided Funtions*/
	virtual void Init() override;
	virtual void Shutdown() override;

	
public:
	/*V.1. Managers*/
	UPROPERTY()
	class UTableManager* TableManager;
	UPROPERTY()
	class UResourceCacheManager* ResourceCacheManager;
	/*V.1. Managers End*/

	/*V.2. Helepers*/
	UPROPERTY()
	UGameStartHelper* GameStartHelper;
	/*V.2. Helepers End*/

};
