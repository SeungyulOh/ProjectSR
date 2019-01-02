// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseCharacter.h"
#include "ProjectSR.h"
#include "Engine.h"
#include "AIModule/Classes/BehaviorTree/BlackboardComponent.h"
#include "NavigationSystem.h"
#include "WidgetLayoutLibrary.h"
#include "Engine/UserInterfaceSettings.h"
#include "TableManager.h"
#include "TableInfos.h"
#include "UtilFunctionLibrary.h"
#include "Tower.h"
#include "AIController.h"
#include "BasePlayerController.h"
#include "TowerSpawner.h"
#include "StageGameMode.h"
#include "EntityRenderComponent.h"

// Sets default values
ABaseCharacter::ABaseCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	DecalMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DecalMeshComponent"));
	DecalMesh->SetupAttachment(RootComponent);

}

// Called when the game starts or when spawned
void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	AIController = GetWorld()->SpawnActor<AAIController>(AIControllerClass);
	AIController->Possess(this);

	

}

// Called every frame
void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ABaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
}

void ABaseCharacter::SetState(ECharacterState InState)
{
	if (CurrentState == InState)
		return;


	CurrentState = InState;
	DoTask();
}

void ABaseCharacter::DoTask()
{
	switch (CurrentState)
	{
	case ECharacterState::ENORMAL:
	{
		if (IsValid(DecalMesh))
			DecalMesh->SetVisibility(false);
	}break;
	case ECharacterState::EWAITINGFORBUILDINGSPOT:
	{
		if (IsValid(DecalMesh))
			DecalMesh->SetVisibility(true);
	}break;
	case ECharacterState::EMOVETOBUILDINGSPOT:
	{
		if (IsValid(DecalMesh))
			DecalMesh->SetVisibility(false);

		
		AAIController* LocalAIController = Cast<AAIController>(GetController());
		if (IsValid(LocalAIController))
		{
			EPathFollowingRequestResult::Type type = LocalAIController->MoveToLocation(TowerBuildingHelper.CandidateTower->GetActorLocation() , 170.f);
			if (type == EPathFollowingRequestResult::RequestSuccessful)
			{
				LocalAIController->ReceiveMoveCompleted.Clear();
				LocalAIController->ReceiveMoveCompleted.AddDynamic(this, &ABaseCharacter::Callback_MoveFinished);
				UE_LOG(LogClass, Log, TEXT("Success"));
			}
		}
	}break;
	case ECharacterState::EPLAYBUILDINGANIM:
	{
		if (IsValid(DecalMesh))
			DecalMesh->SetVisibility(false);

		TowerBuildingHelper.HammerHitCount = 0;

		UUtilFunctionLibrary::GetStageGameMode()->AddGold(-300);

	}break;
	/*case ECharacterState::EBUILDINGCOMPLETED:
	{
		if (IsValid(DecalMesh))
			DecalMesh->SetVisibility(false);

		UEntityRenderComponent* RenderComp = UUtilFunctionLibrary::GetRenderComponent(TowerBuildingHelper.CandidateTower);
		if (IsValid(RenderComp))
		{
			TowerBuildingHelper.CandidateTower->SetActorHiddenInGame(false);

			FEffectStruct effect;
			effect.type = FEffectStruct::EType::EAPPEAR;
			effect.Duration = 2.f;
			RenderComp->AddEffectStruct(effect);
		}

		SetState(ECharacterState::ENORMAL);
	}break;*/
	default:
		break;
	}
}

void ABaseCharacter::Callback_MoveFinished(FAIRequestID RequestID, EPathFollowingResult::Type Result)
{
	UE_LOG(LogClass, Log, TEXT("Callback_MoveFinished"));
	if (Result == EPathFollowingResult::Success)
	{
		SetState(ECharacterState::EPLAYBUILDINGANIM);
	}
}

void FTowerBuildingHelper::OnTouchBegin(FVector Location)
{
	float viewScale = UWidgetLayoutLibrary::GetViewportScale(SRGAMEINSTANCE(GEngine)->GetWorld());
	const FVector2D viewportSize = FVector2D(GEngine->GameViewport->Viewport->GetSizeXY());
	viewScale = GetDefault<UUserInterfaceSettings>(UUserInterfaceSettings::StaticClass())->GetDPIScaleBasedOnSize(FIntPoint(viewportSize.X, viewportSize.Y));
	FVector2D ViewportLoc = FVector2D(Location.X, Location.Y) / viewScale;
	FVector WorldLoc = FVector::ZeroVector;
	UUtilFunctionLibrary::DeprojectViewportPointToNavMesh(ViewportLoc, WorldLoc);

	UTableManager* TableManager = SRGAMEINSTANCE(GEngine)->TableManager;
	FTableInfos* tableinfo = TableManager->GetTableInfo<FTableInfos>(TableManager->DTObjectTable, TEXT("ArrowTower"));
	if (tableinfo)
	{
		if (tableinfo->BlueprintClass.IsValid())
		{
			TSubclassOf<ATower> TowerClass = tableinfo->BlueprintClass.Get();

			FTransform SpawnTransform;
			SpawnTransform.SetLocation(WorldLoc);
			ATower* tower = UUtilFunctionLibrary::GetMyWorld()->SpawnActor<ATower>(TowerClass, SpawnTransform);
			if (IsValid(tower))
			{
				CandidateTower = tower;
				USkeletalMeshComponent* SkeletalMesh = CandidateTower->FindComponentByClass<USkeletalMeshComponent>();
				if (IsValid(SkeletalMesh))
				{
					FResourceCacheInfos* Resourceinfo = TableManager->GetTableInfo<FResourceCacheInfos>(TableManager->DTResourceCacheTable, TEXT("BuildingMat"));
					if (Resourceinfo)
					{
						if (Resourceinfo->CacheObject.IsValid())
						{
							UMaterialInstanceConstant* MIC = Cast<UMaterialInstanceConstant>(Resourceinfo->CacheObject.Get());
							if (MIC)
							{
								SkeletalMesh->SetMaterial(0, MIC);
							}
						}
					}
				}
			}
		}
	}
}

void FTowerBuildingHelper::OnTouchOver(FVector Location)
{
	FVector WorldLocation = FVector::ZeroVector;
	FVector WorldDirection = FVector::ZeroVector;
	FVector TargetVector = FVector::ZeroVector;
	bool bSuccess = UUtilFunctionLibrary::GetBasePlayerController()->DeprojectScreenPositionToWorld(Location.X, Location.Y, WorldLocation, WorldDirection);
	if (bSuccess)
	{
		float CharacterZ = UUtilFunctionLibrary::GetMyCharacter()->GetActorLocation().Z;
		float Delta = (CharacterZ - WorldLocation.Z) / WorldDirection.Z;

		TargetVector = WorldLocation + WorldDirection * Delta;
		UNavigationSystemV1* NavSystem = Cast<UNavigationSystemV1>(UUtilFunctionLibrary::GetMyWorld()->GetNavigationSystem());
		if (NavSystem)
		{
			FNavLocation outdata;
			bool bNavSuccess = NavSystem->ProjectPointToNavigation(TargetVector, outdata);
			if (bNavSuccess)
			{
				TargetVector = outdata.Location;

				if (IsValid(CandidateTower))
				{
					FVector CharacterLocation = UUtilFunctionLibrary::GetMyCharacter()->GetActorLocation();
					CharacterLocation.Z = TargetVector.Z;
					FVector2D Direction = FVector2D(TargetVector.X, TargetVector.Y) - FVector2D(CharacterLocation.X, CharacterLocation.Y);
					float Distance = FMath::Clamp<float>(Direction.Size(), 0, BuildRange / 2.f);
					Direction = Direction.GetSafeNormal();
					FVector NewDirection = FVector(Direction.X, Direction.Y, 0.f);
					CandidateTower->SetActorLocation(CharacterLocation + NewDirection * Distance);
				}
			}
		}
	}
}

void FTowerBuildingHelper::OnTouchEnd(FVector Location)
{
	if (IsValid(CandidateTower))
	{
		CandidateTower->SetActorHiddenInGame(true);
		TowerArray.Emplace(CandidateTower);
		
		ABaseCharacter* BaseCharacter = UUtilFunctionLibrary::GetMyCharacter();
		if (IsValid(BaseCharacter))
		{
			BaseCharacter->SetState(ECharacterState::EMOVETOBUILDINGSPOT);
		}
	}
}

