// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Nexus.generated.h"

UCLASS()
class PROJECTSR_API ANexus : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ANexus();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	UFUNCTION()
	void Callback_BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Nexus")
	class UBoxComponent* BoxComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Nexus")
	class UParticleSystemComponent* ParticleSystemComponent;

	
	
};
