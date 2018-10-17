// Fill out your copyright notice in the Description page of Project Settings.

#include "MonsterSpawner.h"
#include "ProjectSR.h"
#include "Monster.h"
#include "TableManager.h"


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
			UE_LOG(LogClass, Log, TEXT("1"));
			
		}

		--TotalSpawnCount;
		ElapsedTime = 0.f;
	}

}

