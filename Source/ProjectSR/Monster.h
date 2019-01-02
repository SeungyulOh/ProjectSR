// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Monster.generated.h"

UCLASS()
class PROJECTSR_API AMonster : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMonster();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void OnTakeDamage(float Damage);

	void SetHPVisible();

	FVector GetDamageSocketLocation();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ATowerSpawner")
	TSubclassOf<class UUC_HpBar> WidgetClass;
	UPROPERTY()
	class UUC_HpBar* HPBar;

	float HPShowElapsedTime = 1.f;
	float HPShowLifeTime = 1.f;

	float MaxHP = 0;
	float CurHP = 0;
};
