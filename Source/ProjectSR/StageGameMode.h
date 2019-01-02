// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectSRGameModeBase.h"
#include "GlobalContants.h"
#include "StageGameMode.generated.h"

UENUM(BlueprintType)
enum class EGameStateEnum : uint8
{
	IDLE,
	MONSTERSPAWNED,
	STAGECLEAR,
	STAGEFAILED,
	READYTONEXTSTAGE,
	END,
};

/**
 * 
 */
UCLASS()
class PROJECTSR_API AStageGameMode : public AProjectSRGameModeBase
{
	GENERATED_BODY()
	
public:
	AStageGameMode();

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
public:
	UFUNCTION()
	void Callback_MessageAnimationEnd();
	
public:
	void SetUserMode(EUserModeEnum InMode);
	void SetGameStateMode(EGameStateEnum InMode);

	void AddMonsterCount(int32 Count);
	void DecreaseMonsterCount();
	void AddGold(int32 MaxHp);

	FORCEINLINE EUserModeEnum GetCurrentUserMode() { return UserMode; }
	FORCEINLINE EGameStateEnum GetCurrentGameStateMode() { return GameStateMode; }
	FORCEINLINE int32	GetMonsterReaminsCount() { return MonsterRemains; }
	FORCEINLINE int32	GetCurrentStage() { return CurrentStage; }
	FORCEINLINE int32	GetCurrentGold() { return Gold; }

public:
	
	UPROPERTY()
	class AActor* cachedViewtarget = nullptr;

	UPROPERTY()
	class UUP_Ingame* IngameWidget = nullptr;

	UPROPERTY()
	class UUserWidget*	MessageNotifierWidget = nullptr;

	UPROPERTY()
	class UBuildingManager* BuildingManager = nullptr;

	UPROPERTY()
	class ABaseCharacter* BaseCharacter = nullptr;

	UPROPERTY()
	TArray<TWeakObjectPtr<class AMonsterSpawner>> SpawnerArray;

	UPROPERTY()
	TWeakObjectPtr<class APlayerStart>	PlayerStartActor;

	DECLARE_MULTICAST_DELEGATE_OneParam(FMonsterCountChanged , int32)
	FMonsterCountChanged OnMonsterCountChanged;
	DECLARE_MULTICAST_DELEGATE_TwoParams(FGoldChanged, int32 , int32)
	FGoldChanged OnGoldChanged;

private:
	void DoTasks();

private:
	UPROPERTY()
	EUserModeEnum		UserMode = EUserModeEnum::ENORMAL;

	UPROPERTY()
	EGameStateEnum		GameStateMode = EGameStateEnum::IDLE;

	bool				bMonsterSpawned = false;

	//stage info
	int32				CurrentStage = 1;
	int32				MonsterRemains = 0;
	int32				Gold = 1000;
	
};


UCLASS()
class PROJECTSR_API UBuildingManager : public UObject
{
	GENERATED_BODY()


public:
	void TouchBegin(FVector location);
	void TouchOver(FVector location);
	void TouchEnd(FVector location);

	void FirstSpawn();
	void CancelSpawn();

	void OnWallsRefreshed(class ASplineWall* RefreshedWall);

public:
	DECLARE_MULTICAST_DELEGATE_OneParam(FExpectGoldConsumption, int32)
	FExpectGoldConsumption OnExpectGoldConsumption;

public:
	UPROPERTY()
	TArray<class ASplineWall*> WallArray;

	UPROPERTY()
	TWeakObjectPtr<class ASplineWall>	SpawnedWalllately;
	UPROPERTY()
	TWeakObjectPtr<class ASplineWall>	SelectedWalllately;

	TArray<FVector> WallPoints;

	FVector SelectedPointonNavMesh = FVector::ZeroVector;

	int32 ReqGold = 0;

};