// Fill out your copyright notice in the Description page of Project Settings.

#include "MonsterSpawner.h"
#include "ProjectSR.h"
#include "Monster.h"
#include "TableManager.h"
#include "NavigationSystem.h"
#include "NavigationSystem/Public/NavigationPath.h"


// Sets default values
AMonsterSpawner::AMonsterSpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));

	ParticleSystemComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ParticleSystemComponent"));
	ParticleSystemComponent->SetupAttachment(RootComponent);

}

// Called when the game starts or when spawned
void AMonsterSpawner::BeginPlay()
{
	Super::BeginPlay();

	FTableInfos* TableInfos = SRGAMEINSTANCE(this)->TableManager->GetTableInfo<FTableInfos>(SRGAMEINSTANCE(this)->TableManager->DTCharacterTable, TEXT("Orc"));
	if (TableInfos && TableInfos->BlueprintClass.IsValid())
	{
		MonsterClass = TableInfos->BlueprintClass.Get();
	}
	else
	{
#ifdef WITH_EDITOR
		MonsterClass = TableInfos->BlueprintClass.LoadSynchronous();
#endif
	}

	TArray<AActor*> OutActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), OutActors);
	FVector TargetLocation = OutActors[0]->GetActorLocation();

	
	UNavigationSystemV1* NavSystem = Cast<UNavigationSystemV1>(GetWorld()->GetNavigationSystem());
	if (NavSystem)
	{
		UNavigationPath* path = NavSystem->FindPathToLocationSynchronously(GetWorld(), GetActorLocation(), TargetLocation);
		if (path)
		{
			path->EnableDebugDrawing(true);
			TArray<FNavPathPoint> pathPoints = path->GetPath()->GetPathPoints();

			for (size_t i = 0; i < pathPoints.Num(); ++i)
			{
				if (!pathPoints.IsValidIndex(i + 1))
					break;

				FVector StartPoint = pathPoints[i].Location;
				FVector EndPoint = pathPoints[i + 1].Location;
				DrawDebugDirectionalArrow(GetWorld(), StartPoint, EndPoint, 10.f, FColor::Cyan, false, 10.f, 0.f, 10.f);
			}

			
		}
	}
	
}

// Called every frame
void AMonsterSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsPendingKill())
		return;

	if (TotalSpawnCount <= 0)
		Destroy();

	ElapsedTime += DeltaTime;
	if (ElapsedTime >= SpawnCoolTime)
	{
		FTransform SpawnTransform;
		SpawnTransform.SetLocation(GetActorLocation());

		for (size_t i = 0; i < SpawnMonsterAtOnce; ++i)
		{
			AMonster* Monster =  GetWorld()->SpawnActor<AMonster>(MonsterClass, SpawnTransform);
			UE_LOG(LogClass, Log, TEXT("SpawnMonster : %s") , *Monster->GetName());
			
		}

		--TotalSpawnCount;
		ElapsedTime = 0.f;
	}

}

