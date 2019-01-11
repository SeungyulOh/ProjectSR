// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Monster.generated.h"

UENUM(BlueprintType)
enum class EMonsterAbnormalState : uint8
{
	IDLE				UMETA(DisplayName = "IDLE"),
	STUNNED				UMETA(DisplayName = "STUNNED"),
	END					UMETA(DisplayName = "END")
};

UENUM(BlueprintType)
enum class EAnimState : uint8
{
	ANIMIDLE				UMETA(DisplayName = "ANIMIDLE"),
	ANIMRUN					UMETA(DisplayName = "ANIMRUN"),
	ANIMSTUNNED				UMETA(DisplayName = "ANIMSTUNNED"),
	ANIMEND					UMETA(DisplayName = "ANIMEND")
};

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

	void SetAbnormalState(EMonsterAbnormalState InState, float Duration);

	FVector GetDamageSocketLocation();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AMonster")
	TSubclassOf<class UUC_HpBar> WidgetClass;
	
	UPROPERTY()
	class UUC_HpBar* HPBar;

	/*Abnormal Related*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AMonster")
	EMonsterAbnormalState	AbnormalState;
	float AbnormalElapsedTime = 0.f;
	float AbnormalDuration = 0.f;
	/*Abnormal Related End*/

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AMonster")
	EAnimState	AnimState = EAnimState::ANIMIDLE;

	float HPShowElapsedTime = 1.f;
	float HPShowLifeTime = 1.f;

	float MaxHP = 0;
	float CurHP = 0;
};
