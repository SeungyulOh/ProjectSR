// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SplineWall.generated.h"

UCLASS()
class PROJECTSR_API ASplineWall : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASplineWall();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void Refresh(TArray<FVector> InPoints);

public:
	UPROPERTY(VisibleAnywhere , BlueprintReadOnly , Category = "ASplineWall")
	class USplineComponent*		SplineComponent;

	TArray<class USplineMeshComponent*> SplineMeshArray;
	
	TWeakObjectPtr<class UStaticMesh> CachedWallMesh;

	UPROPERTY()
	TArray<FVector>	SplinePointVectors;
};
