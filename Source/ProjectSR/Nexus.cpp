// Fill out your copyright notice in the Description page of Project Settings.

#include "Nexus.h"
#include "Engine.h"
#include "Monster.h"
#include "UC_HpBar.h"

// Sets default values
ANexus::ANexus()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	BoxComponent->SetupAttachment(RootComponent);

	ParticleSystemComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ParticleSystemComponent"));
	ParticleSystemComponent->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ANexus::BeginPlay()
{
	Super::BeginPlay();

	BoxComponent->OnComponentBeginOverlap.Clear();
	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &ANexus::Callback_BeginOverlap);
	
}

// Called every frame
void ANexus::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ANexus::Callback_BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (IsValid(OtherActor))
	{
		if (OtherActor->GetClass()->IsChildOf<AMonster>())
		{
			AMonster* Monster = Cast<AMonster>(OtherActor);
			if (IsValid(Monster))
				Monster->HPBar->RemoveFromViewport();
			OtherActor->Destroy();
		}
			
	}
}

