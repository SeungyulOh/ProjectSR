// Fill out your copyright notice in the Description page of Project Settings.


#include "Tower.h"
#include "ProjectSR.h"
#include "Monster.h"
#include "BinarySearch.h"
#include "EntityRenderComponent.h"
#include "Kismet/KismetMathLibrary.h"


// Sets default values
ATower::ATower()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComp"));

	SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalComp"));
	SkeletalMeshComponent->SetupAttachment(RootComponent);

	Sight = CreateDefaultSubobject<USphereComponent>(TEXT("Sight"));
	Sight->SetupAttachment(RootComponent);

	Collision = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Collision"));
	Collision->SetupAttachment(RootComponent);

	EntityRenderComponent = CreateDefaultSubobject<UEntityRenderComponent>(TEXT("EntityRenderComponent"));
}

// Called when the game starts or when spawned
void ATower::BeginPlay()
{
	Super::BeginPlay();

	if (IsValid(Sight))
	{
		Sight->SetSphereRadius(SightRange / 2.f);
		Sight->OnComponentBeginOverlap.Clear();
		Sight->OnComponentBeginOverlap.AddDynamic(this, &ATower::Callback_BeginOverlap);
	}
		
	
}

// Called every frame
void ATower::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (MonsterCandidates.IsValidIndex(0))
	{
		if (MonsterCandidates[0].IsValid())
		{
			FRotator LookRot = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), MonsterCandidates[0]->GetActorLocation());
			LookRot.Roll = 0.f;
			LookRot.Pitch = 0.f;
			FRotator CurrentRot = GetActorRotation();

			FRotator modifiedRot = FMath::RInterpTo(CurrentRot, LookRot, DeltaTime, 10.f);
			SetActorRotation(modifiedRot);
		}
	}
	

}

void ATower::Callback_BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	AMonster* TargetMonster = Cast<AMonster>(OtherActor);
	if (IsValid(TargetMonster))
	{
		MonsterCandidates.AddUnique(TargetMonster);
	}
}

class AMonster* ATower::SearchClosestMonster()
{
	for (size_t i = 0; i < MonsterCandidates.Num(); )
	{
		if (MonsterCandidates.IsValidIndex(i))
		{
			if (!MonsterCandidates[i].IsValid())
			{
				MonsterCandidates.RemoveAt(i);
				continue;
			}
		}

		++i;
	}

	Algo::Sort(MonsterCandidates, [this](TWeakObjectPtr<AMonster> Source , TWeakObjectPtr<AMonster> Dest) {
		FVector2D TowerLocation = FVector2D(GetActorLocation().X, GetActorLocation().Y);
		
		FVector2D SorceLocation = FVector2D(Source->GetActorLocation().X, Source->GetActorLocation().Y);
		FVector2D DestLocation = FVector2D(Dest->GetActorLocation().X, Dest->GetActorLocation().Y);

		return (SorceLocation - TowerLocation).Size() < (DestLocation - TowerLocation).Size();
	});

	if (MonsterCandidates.IsValidIndex(0) && MonsterCandidates[0].IsValid())
		return MonsterCandidates[0].Get();

	return nullptr;
}

class AMonster* ATower::GetTargetMonster()
{
	if (MonsterCandidates.IsValidIndex(0))
	{
		if (MonsterCandidates[0].IsValid())
			return MonsterCandidates[0].Get();
	}

	return nullptr;
}

