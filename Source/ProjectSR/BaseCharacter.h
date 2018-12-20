// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Navigation/PathFollowingComponent.h"
#include "AITypes.h"
#include "BaseCharacter.generated.h"

UENUM(BlueprintType)
enum class ECharacterState : uint8
{
	ENORMAL,
	EWAITINGFORBUILDINGSPOT,
	EMOVETOBUILDINGSPOT,
	EPLAYBUILDINGANIM,
	EEND,
};
USTRUCT(BlueprintType)
struct PROJECTSR_API FTowerBuildingHelper
{
	GENERATED_USTRUCT_BODY()

public:
	void OnTouchBegin(FVector Location);
	void OnTouchOver(FVector Location);
	void OnTouchEnd(FVector Location);

public:
	UPROPERTY()
	TArray<class ATower*> TowerArray;

	UPROPERTY()
	TArray<class ATowerSpawner*> TowerSpawnerArray;

	UPROPERTY()
	class ATower* CandidateTower;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "FTowerBuildingHelper")
	int32 HammerHitCount = 0;


private:
	float BuildRange = 1000.f;


};

UCLASS()
class PROJECTSR_API ABaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABaseCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void SetState(ECharacterState InState);
	FORCEINLINE ECharacterState GetCurrentState() { return CurrentState; }

private:
	void DoTask();

	UFUNCTION()
	void Callback_MoveFinished(FAIRequestID RequestID, EPathFollowingResult::Type Result);
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BaseCharacter")
	class UStaticMeshComponent* DecalMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BaseCharacter")
	ECharacterState CurrentState = ECharacterState::ENORMAL;

	UPROPERTY(VisibleAnywhere , BlueprintReadOnly , Category = "BaseCharacter")
	FTowerBuildingHelper	TowerBuildingHelper;

	UPROPERTY()
	class AAIController*	AIController;

private:
	float InitialDecalRadius = 100.f;
	

	
	
};
