// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TowerSpawner.generated.h"

UENUM(BlueprintType)
enum class ETowerSpawnerState : uint8
{
	ESPAWNING,
	EWORKING,
	EDISAPPEARING,
	EEND,
};



UCLASS()
class PROJECTSR_API ATowerSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATowerSpawner();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ATowerSpawner")
	class UParticleSystemComponent* ParticleSystemComponent = nullptr;

	UPROPERTY()
	class ATower* TargetTower = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ATowerSpawner")
	TSubclassOf<class UUC_HpBar> WidgetClass;

	UPROPERTY()
	class UUC_HpBar* ProgressWidget;
	UPROPERTY()
	class UCurveFloat* BuildingCurve;

	ETowerSpawnerState CurrentState = ETowerSpawnerState::EEND;

	float Duration = 5.f;
	float CurveActiveTime = 0.f;

	
	
};
