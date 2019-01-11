// Fill out your copyright notice in the Description page of Project Settings.

#include "ItemSpawner.h"
#include "ProjectSR.h"
#include "Components/BoxComponent.h"
#include "TableInfos.h"
#include "TableManager.h"
#include "Gold3D.h"


// Sets default values
AItemSpawner::AItemSpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AItemSpawner::BeginPlay()
{
	Super::BeginPlay();

	UBoxComponent* BoxComponent = FindComponentByClass<UBoxComponent>();
	FVector WorldLoc = BoxComponent->GetComponentLocation();
	FVector Extent = BoxComponent->GetScaledBoxExtent();

	FVector MinPoint = WorldLoc;
	MinPoint.X -= Extent.X * 0.5f;
	MinPoint.Y -= Extent.Y * 0.5f;

	FVector MaxPoint = WorldLoc;
	MaxPoint.X += Extent.X * 0.5f;
	MaxPoint.Y += Extent.Y * 0.5f;
	FBox SpawnerBox(MinPoint, MaxPoint);

	FVector ResultVector = FMath::RandPointInBox(SpawnerBox);

	TSubclassOf<AGold3D> GoldClass = nullptr;
	FTableInfos* TableInfos = SRGAMEINSTANCE(this)->TableManager->GetTableInfo<FTableInfos>(SRGAMEINSTANCE(this)->TableManager->DTObjectTable, TEXT("Gold"));
	if (TableInfos && TableInfos->BlueprintClass.IsValid())
	{
		GoldClass = TableInfos->BlueprintClass.Get();
	}
	else
	{
#ifdef WITH_EDITOR
		GoldClass = TableInfos->BlueprintClass.LoadSynchronous();
#endif
	}

	FTransform SpawnTransform;
	SpawnTransform.SetLocation(ResultVector);
	AGold3D* Gold = GetWorld()->SpawnActor<AGold3D>(GoldClass, SpawnTransform);
}

// Called every frame
void AItemSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

