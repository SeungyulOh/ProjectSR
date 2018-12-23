// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Tower.generated.h"

UCLASS()
class PROJECTSR_API ATower : public APawn
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATower();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void Callback_BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UFUNCTION(BlueprintCallable)
	class AMonster* SearchClosestMonster();
	UFUNCTION(BlueprintImplementableEvent)
	void	TowerActivated();

	class AMonster* GetTargetMonster();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tower")
	class USkeletalMeshComponent* SkeletalMeshComponent = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tower")
	class UCapsuleComponent* Collision = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tower")
	class USphereComponent* Sight = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tower")
	float SightRange = 2000.f;

	

	UPROPERTY()
	class UEntityRenderComponent* EntityRenderComponent;

private:
	UPROPERTY()
	TArray<TWeakObjectPtr<class AMonster>> MonsterCandidates;
	
	
};
