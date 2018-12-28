// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectSRGameModeBase.h"
#include "GlobalContants.h"
#include "StageGameMode.generated.h"

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
	void SetUserMode(EUserModeEnum InMode);

	FORCEINLINE EUserModeEnum GetCurrentUserMode() { return UserMode; }
	FORCEINLINE bool			GetisMonsterSpawned() { return bMonsterSpawned; }
	FORCEINLINE void			SetisMonsterSpawned() { bMonsterSpawned = true; }

public:
	
	UPROPERTY()
	class AActor* cachedViewtarget = nullptr;

	UPROPERTY()
	class UUP_Ingame* IngameWidget = nullptr;

	UPROPERTY()
	class UBuildingManager* BuildingManager = nullptr;

	UPROPERTY()
	class ABaseCharacter* BaseCharacter = nullptr;

	UPROPERTY()
	TArray<TWeakObjectPtr<class AMonsterSpawner>> SpawnerArray;

	UPROPERTY()
	TWeakObjectPtr<class APlayerStart>	PlayerStartActor;

private:
	void DoTasks();

private:
	UPROPERTY()
	EUserModeEnum		UserMode = EUserModeEnum::ENORMAL;

	bool				bMonsterSpawned = false;
	
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

public:
	UPROPERTY()
	TArray<class ASplineWall*> WallArray;

	UPROPERTY()
	TWeakObjectPtr<class ASplineWall>	SpawnedWalllately;
	UPROPERTY()
	TWeakObjectPtr<class ASplineWall>	SelectedWalllately;

	TArray<FVector> WallPoints;

	FVector SelectedPointonNavMesh = FVector::ZeroVector;

};