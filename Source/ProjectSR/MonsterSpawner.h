// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MonsterSpawner.generated.h"

UCLASS()
class PROJECTSR_API AMonsterSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMonsterSpawner();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


public:
	/*This Spawner is done with working when TotalSpawnCount is zero*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AMonsterSpawner")
	int32 TotalSpawnCount = 0;
	/*How many Monsters do you want to spawn at once*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AMonsterSpawner")
	int32 SpawnMonsterAtOnce = 0;
	/*Spawn Period*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AMonsterSpawner")
	float SpawnCoolTime = 0.f;

private:
	float ElapsedTime = 0.f;
	
};
