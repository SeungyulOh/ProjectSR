// Fill out your copyright notice in the Description page of Project Settings.

#include "Gold3D.h"
#include "ProjectSR.h"
#include "Engine/BlueprintGeneratedClass.h"
#include "Monster.h"
#include "UtilFunctionLibrary.h"
#include "StageGameMode.h"
#include "ObservableManager.h"
#include "Observable_StageData.h"

// Sets default values
AGold3D::AGold3D()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AGold3D::BeginPlay()
{
	Super::BeginPlay();

	UBoxComponent* BoxComponent = FindComponentByClass<UBoxComponent>();
	if (IsValid(BoxComponent))
	{
		BoxComponent->OnComponentBeginOverlap.Clear();
		BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &AGold3D::Callback_BeginOverlap);
	}

	ObservableStageData = UObservableManager::Get()->GetObservable<UObservable_StageData>();
	if (ObservableStageData.IsValid())
	{
		ObservableStageData->Register(this);
	}
}

// Called every frame
void AGold3D::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AGold3D::Update()
{

}

void AGold3D::Callback_BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	AMonster* Monster = Cast<AMonster>(OtherActor);
	if (IsValid(Monster))
	{
		int32 CurrentStage = ObservableStageData->GetStageData().CurrentStage;
		int32 Gold = UUtilFunctionLibrary::GetInitialGoldEquivalant(CurrentStage);
		float finalGold = FMath::RandRange(Gold * 0.1f, Gold * 0.5f);
		UUtilFunctionLibrary::GetStageGameMode()->AddGold(finalGold);
		Destroy();
	}
}

