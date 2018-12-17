// Fill out your copyright notice in the Description page of Project Settings.

#include "Projectile.h"
#include "ProjectSR.h"
#include "Monster.h"
#include "TableManager.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
AProjectile::AProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	RootComponent = StaticMeshComponent;

	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComp"));
	CapsuleComponent->SetupAttachment(RootComponent);
	
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));

}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();

	if (IsValid(CapsuleComponent))
	{
		CapsuleComponent->OnComponentBeginOverlap.Clear();
		CapsuleComponent->OnComponentBeginOverlap.AddDynamic(this, &AProjectile::Callback_BeginOverlap);
	}
	
}

// Called every frame
void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsPendingKill())
		return;

	ElapsedTime += DeltaTime;

	if (ElapsedTime > LifeTime)
		Destroy();

}

void AProjectile::Callback_BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	AMonster* TargetMonster = Cast<AMonster>(OtherActor);
	if (IsValid(TargetMonster))
	{
		UTableManager* TableManager = SRGAMEINSTANCE(this)->TableManager;
		if (TableManager)
		{
			FResourceCacheInfos* tableinfo = TableManager->GetTableInfo<FResourceCacheInfos>(TableManager->DTResourceCacheTable, TEXT("ArrowHit"));
			if (tableinfo)
			{
				if (tableinfo->CacheObject.IsValid())
				{
					UParticleSystem* ParticleSystem = Cast<UParticleSystem>(tableinfo->CacheObject.Get());
					if (ParticleSystem)
					{
						FTransform spawntransform;
						USkeletalMeshComponent* targetskeletalcomp = Cast<USkeletalMeshComponent>(TargetMonster->GetComponentByClass(USkeletalMeshComponent::StaticClass()));
						if (IsValid(targetskeletalcomp))
						{
							FVector DamageVector = targetskeletalcomp->GetSocketLocation(TEXT("Socket_Damage"));
							spawntransform.SetTranslation(DamageVector);
							UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ParticleSystem, spawntransform);
						}
					}
				}
			}
		}

		CapsuleComponent->OnComponentBeginOverlap.Clear();
		TargetMonster->OnTakeDamage(Damage);
		Destroy();
	}
}

