// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SRObserver.h"
#include "Gold3D.generated.h"

UCLASS()
class PROJECTSR_API AGold3D : public AActor , public ISRObserver
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGold3D();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void Update();

	UFUNCTION()
	void Callback_BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

private:
	UPROPERTY()
	TWeakObjectPtr<class UObservable_StageData> ObservableStageData;
	
};
