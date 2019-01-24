// Fill out your copyright notice in the Description page of Project Settings.

#include "UtilFunctionLibrary.h"
#include "Camera/CameraActor.h"
#include "ProjectSR.h"
#include "BaseLevelScriptActor.h"
#include "BasePlayerController.h"
#include "StageGameMode.h"
#include "UserWidget.h"
#include "Engine/UserInterfaceSettings.h"
#include "WidgetLayoutLibrary.h"
#include "NavigationSystem.h"
#include "BaseCharacter.h"
#include "EntityRenderComponent.h"
#include "MonsterSpawner.h"
#include "Monster.h"
#include "StageGameHUD.h"
#include "NavigationPath.h"




ACameraActor* UUtilFunctionLibrary::GetDynamicCamera()
{
	ABaseLevelScriptActor* BaseLevelScriptActor = Cast<ABaseLevelScriptActor>(SRGAMEINSTANCE(GEngine)->GetWorld()->GetLevelScriptActor());
	if (IsValid(BaseLevelScriptActor))
		return BaseLevelScriptActor->DynamicCamera;


	return nullptr;
}

AActor* UUtilFunctionLibrary::GetBaseCamera()
{
	ABaseLevelScriptActor* BaseLevelScriptActor = Cast<ABaseLevelScriptActor>(SRGAMEINSTANCE(GEngine)->GetWorld()->GetLevelScriptActor());
	if (IsValid(BaseLevelScriptActor))
		return BaseLevelScriptActor->BaseCamera;


	return nullptr;
}

UWorld* UUtilFunctionLibrary::GetMyWorld()
{
	if(GEngine)
		return SRGAMEINSTANCE(GEngine)->GetWorld();

	return nullptr;
}

ABasePlayerController* UUtilFunctionLibrary::GetBasePlayerController()
{
	return Cast<ABasePlayerController>(UGameplayStatics::GetPlayerController(SRGAMEINSTANCE(GEngine)->GetWorld() , 0));
}

class AStageGameMode* UUtilFunctionLibrary::GetStageGameMode()
{
	return Cast<AStageGameMode>(UGameplayStatics::GetGameMode(SRGAMEINSTANCE(GEngine)->GetWorld()));
}

class AStageGameHUD* UUtilFunctionLibrary::GetStageGameHUD()
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(SRGAMEINSTANCE(GEngine)->GetWorld(), 0);
	if (IsValid(PC))
	{
		return Cast<AStageGameHUD>(PC->GetHUD());
	}
	
	return nullptr;
}

class UEntityRenderComponent* UUtilFunctionLibrary::GetRenderComponent(AActor* TargetActor)
{
	UEntityRenderComponent* RenderComponent = TargetActor->FindComponentByClass<UEntityRenderComponent>();
	
	return RenderComponent;
}

class ABaseCharacter* UUtilFunctionLibrary::GetMyCharacter()
{
	//ABaseCharacter* BaseCharacter = Cast<ABaseCharacter>(UGameplayStatics::GetPlayerPawn(SRGAMEINSTANCE(GEngine)->GetWorld(), 0));
	ABaseCharacter* BaseCharacter = UUtilFunctionLibrary::GetStageGameMode()->BaseCharacter;
	return BaseCharacter;
}

void UUtilFunctionLibrary::StageFail()
{
	UUtilFunctionLibrary::GetStageGameMode()->SetGameStateMode(EGameStateEnum::STAGEFAILED);
}

class ABaseLevelScriptActor* UUtilFunctionLibrary::GetBaseLevelScriptActor()
{
	UWorld* world = SRGAMEINSTANCE(GEngine)->GetWorld();
	if (IsValid(world))
	{
		return Cast<ABaseLevelScriptActor>(world->GetLevelScriptActor());
	}

	return nullptr;
}

class UBuildingManager* UUtilFunctionLibrary::GetBuildingManager()
{
	AStageGameMode* StageGameMode = Cast<AStageGameMode>(UGameplayStatics::GetGameMode(SRGAMEINSTANCE(GEngine)->GetWorld()));
	if (IsValid(StageGameMode))
		return StageGameMode->BuildingManager;

	return nullptr;
}

void UUtilFunctionLibrary::PlayWidgetAnimation(UUserWidget* widget, FString AnimnName, bool bLoop , EUMGSequencePlayMode::Type type)
{
	UWidgetBlueprintGeneratedClass* WidgetBlueprintClass = Cast<UWidgetBlueprintGeneratedClass>(widget->GetClass());
	if (WidgetBlueprintClass)
	{
		UWidgetAnimation** FoundAnim = WidgetBlueprintClass->Animations.FindByPredicate([AnimnName](UWidgetAnimation* anim) {
			if (IsValid(anim))
			{
				if (anim->GetName().Contains(AnimnName))
					return true;
			}
			return false;
		});

		if (IsValid(*FoundAnim))
			widget->PlayAnimation(*FoundAnim , 0.f, !bLoop , type);
	}
}

class UWidgetAnimation* UUtilFunctionLibrary::GetWidgetAnimation(UUserWidget* widget, FString AnimnName)
{
	UWidgetBlueprintGeneratedClass* WidgetBlueprintClass = Cast<UWidgetBlueprintGeneratedClass>(widget->GetClass());
	if (WidgetBlueprintClass)
	{
		UWidgetAnimation** FoundAnim = WidgetBlueprintClass->Animations.FindByPredicate([AnimnName](UWidgetAnimation* anim) {
			if (IsValid(anim))
			{
				if (anim->GetName().Contains(AnimnName))
					return true;
			}
			return false;
		});

		if (IsValid(*FoundAnim))
			return *FoundAnim;
	}

	return nullptr;
}

bool UUtilFunctionLibrary::DeprojectViewportPointToNavMesh(FVector2D viewportLoc, FVector& outLoc)
{
	float viewScale = UWidgetLayoutLibrary::GetViewportScale(SRGAMEINSTANCE(GEngine)->GetWorld());
	const FVector2D viewportSize = FVector2D(GEngine->GameViewport->Viewport->GetSizeXY());
	viewScale = GetDefault<UUserInterfaceSettings>(UUserInterfaceSettings::StaticClass())->GetDPIScaleBasedOnSize(FIntPoint(viewportSize.X, viewportSize.Y));
	viewportLoc *= viewScale;

	FVector WorldLocation = FVector::ZeroVector;
	FVector WorldDirection = FVector::ZeroVector;
	bool bSuccess = UUtilFunctionLibrary::GetBasePlayerController()->DeprojectScreenPositionToWorld(viewportLoc.X, viewportLoc.Y, WorldLocation, WorldDirection);
	if (bSuccess)
	{
		TArray<FHitResult> outResult;
		UUtilFunctionLibrary::GetMyWorld()->LineTraceMultiByChannel(outResult, WorldLocation, WorldLocation + WorldDirection * 10000, ECollisionChannel::ECC_WorldStatic);
		for (size_t i = 0; i < outResult.Num(); ++i)
		{
			ABlockingVolume* BlockingVolume = Cast<ABlockingVolume>(outResult[i].GetActor());
			if (BlockingVolume)
			{
				UNavigationSystemV1* NavSystem = Cast<UNavigationSystemV1>(UUtilFunctionLibrary::GetMyWorld()->GetNavigationSystem());
				if (NavSystem)
				{
					FNavLocation outdata;
					bool bNavSuccess = NavSystem->ProjectPointToNavigation(outResult[i].ImpactPoint, outdata);
					if (bNavSuccess)
					{
						outLoc = outdata.Location;
						return true;
					}
				}
			}
		}
	}

	return false;
}

bool UUtilFunctionLibrary::isPartialPath()
{
	AStageGameMode* StageGameMode = UUtilFunctionLibrary::GetStageGameMode();
	if (!IsValid(StageGameMode))
		return true;
	
	if (!StageGameMode->PlayerStartActor.IsValid())
		return true;
			
	FVector TargetPoint = StageGameMode->PlayerStartActor->GetActorLocation();
	for (size_t i = 0; i < StageGameMode->SpawnerArray.Num(); ++i)
	{
		if (StageGameMode->SpawnerArray[i].IsValid())
		{
			FVector StartingPoint = StageGameMode->SpawnerArray[i]->GetActorLocation();

			UNavigationSystemV1* NavSystem = Cast<UNavigationSystemV1>(StageGameMode->GetWorld()->GetNavigationSystem());
			if (NavSystem)
			{
				UNavigationPath* path = NavSystem->FindPathToLocationSynchronously(StageGameMode->GetWorld(), StartingPoint, TargetPoint);
				if (path)
				{
					if (path->IsPartial())
						return true;
				}
			}
		}
	}

	return false;
}

void UUtilFunctionLibrary::SearchMonster(TArray<class AMonster*>& monsters, FVector CenterPoint, float Radius)
{
	TArray<TWeakObjectPtr<class AMonster>>& MonsterArray = UUtilFunctionLibrary::GetStageGameMode()->GetMonsterArray();

	for (size_t i = 0; i < MonsterArray.Num(); ++i)
	{
		if (!MonsterArray[i].IsValid())
			continue;

		FVector targetVector = MonsterArray[i]->GetActorLocation();
		FVector Dir = targetVector - CenterPoint;
		if (Dir.Size2D() < Radius)
			monsters.Emplace(MonsterArray[i].Get());
	}
}

int32 UUtilFunctionLibrary::GetRequiredGold(float WallLength)
{
	return WallLength * 0.1f;
}

int32 UUtilFunctionLibrary::GetInitialGoldEquivalant(int32 CurrentStage)
{
	return FMath::Pow(1.2f, CurrentStage-1) * 1000;
}
