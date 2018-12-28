// Fill out your copyright notice in the Description page of Project Settings.

#include "SplineWall.h"
#include "ProjectSR.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "TableManager.h"


// Sets default values
ASplineWall::ASplineWall()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SplineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("SplineComponent"));
	SplineComponent->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ASplineWall::BeginPlay()
{
	Super::BeginPlay();

	UTableManager* TableManager = SRGAMEINSTANCE(this)->TableManager;
	if (TableManager)
	{
		FResourceCacheInfos* tableinfo = TableManager->GetTableInfo<FResourceCacheInfos>(TableManager->DTResourceCacheTable, TEXT("Wall"));
		if (tableinfo)
		{
			if (tableinfo->CacheObject.IsValid())
			{
				UStaticMesh* SM = Cast<UStaticMesh>(tableinfo->CacheObject.Get());
				if (SM)
					CachedWallMesh = SM;
			}
		}
	}
	
}

// Called every frame
void ASplineWall::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASplineWall::Refresh(TArray<FVector> InPoints)
{
	if (IsValid(SplineComponent))
	{
		for (size_t i = 0; i < SplineMeshArray.Num(); ++i)
		{
			SplineMeshArray[i]->DestroyComponent();
		}
		SplineMeshArray.Empty();
		SplineComponent->ClearSplinePoints();

		for (size_t i = 0; i < InPoints.Num(); ++i)
		{
			SplineComponent->AddSplinePoint(InPoints[i] , ESplineCoordinateSpace::World);
		}

		int32 TotalPoints = SplineComponent->GetNumberOfSplinePoints();
		int32 MeshCount = TotalPoints - 1;

		for (size_t i = 0; i < MeshCount; ++i)
		{
			USplineMeshComponent* meshComponent = NewObject<USplineMeshComponent>(this);
			if (meshComponent)
			{
				meshComponent->OnComponentCreated();
				meshComponent->SetForwardAxis(ESplineMeshAxis::X);
				//meshComponent->SetMobility(EComponentMobility::Movable);

				if (CachedWallMesh.IsValid())
					meshComponent->SetStaticMesh(CachedWallMesh.Get());

				

				FVector StartPos = SplineComponent->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::World);
				FVector StartTangent = SplineComponent->GetTangentAtSplinePoint(i, ESplineCoordinateSpace::World);

				FVector EndPos = SplineComponent->GetLocationAtSplinePoint(i+1, ESplineCoordinateSpace::World);
				FVector EndTangent = SplineComponent->GetTangentAtSplinePoint(i+1, ESplineCoordinateSpace::World);

				meshComponent->SetStartAndEnd(StartPos, StartTangent, EndPos, EndTangent);
				meshComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

				//meshComponent->AttachToComponent(SplineComponent, FAttachmentTransformRules::KeepRelativeTransform);
				meshComponent->RegisterComponent();
				SplineMeshArray.Emplace(meshComponent);
			}
		}
	}
}

