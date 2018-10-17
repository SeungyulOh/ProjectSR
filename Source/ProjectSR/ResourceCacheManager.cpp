// Fill out your copyright notice in the Description page of Project Settings.

#include "ResourceCacheManager.h"
#include "ProjectSR.h"
#include "TableManager.h"
#include "Engine/AssetManager.h"
#include "UP_LoadingScene.h"

const float MapLoadPortion = 0.2f;

void UResourceCacheManager::Init()
{
	USRGameInstance* GameInst = Cast<USRGameInstance>(GetOuter());
	if (GameInst)
	{
		FWidgetTableInfos* TableInfo = GameInst->TableManager->GetTableInfo<FWidgetTableInfos>(GameInst->TableManager->DTWidgetTable, TEXT("UP_LoadingScene"));
		if (TableInfo)
		{
			UClass* WidgetClass = nullptr;
			if (TableInfo->WidgetClass.IsValid())
			{
				WidgetClass = TableInfo->WidgetClass.Get();
			}
			else
			{
				WidgetClass = TableInfo->WidgetClass.LoadSynchronous();
			}

			LoadingWidget = CreateWidget<UUP_LoadingScene>(SRGAMEINSTANCE(GEngine), WidgetClass, TEXT("UP_Loading"));
		}
	}
	
}

void UResourceCacheManager::Cache_Map()
{
	ShowLoadingScene(true);

	

	USRGameInstance* GameInst = Cast<USRGameInstance>(GetOuter());
	if (GameInst)
	{
		int32 CurrentStage = GameInst->GameStartHelper->CurrentStage;

		UTableManager* TableManager = GameInst->TableManager;
		FMapTableInfos* MapTableInfo = TableManager->GetTableInfo<FMapTableInfos>(TableManager->DTMapTable, FName(*FString::FromInt(CurrentStage)));
		if (MapTableInfo)
		{
			FSoftObjectPath path = MapTableInfo->MapObjectPtr.ToSoftObjectPath();

			LoadingWidget->ResourceText->SetText(FText::FromString(path.ToString()));
			
			FLoadPackageAsyncDelegate Delegates;
			Delegates.BindLambda([this , GameInst, MapTableInfo](const FName& PackageName, UPackage* LoadedPackage, EAsyncLoadingResult::Type Result) {
				if (Result == EAsyncLoadingResult::Succeeded)
				{
					UE_LOG(LogClass, Log, TEXT("%s"), *PackageName.ToString());
					CachedMap = LoadedPackage;
					
					OnResourceCacheFinished.ExecuteIfBound((uint8)UGameStartHelper::EPrerequisiteGameStart::MAPCACHE);
				}
			});
			LoadPackageAsync(path.GetLongPackageName(), Delegates);
		}
	}
}

void UResourceCacheManager::Cache_Remains()
{
	//MapLoaded
	LoadingWidget->ProgressBar->SetPercent(MapLoadPortion);

	USRGameInstance* GameInst = Cast<USRGameInstance>(GetOuter());
	if (GameInst)
	{
		int32 CurrentStage = GameInst->GameStartHelper->CurrentStage;

		UTableManager* TableManager = GameInst->TableManager;
		FMapTableInfos* MapTableInfo = TableManager->GetTableInfo<FMapTableInfos>(TableManager->DTMapTable, FName(*FString::FromInt(CurrentStage)));
		if (MapTableInfo)
		{
			for (size_t i = 0; i < MapTableInfo->ActorIDList.Num(); ++i)
			{
				FTableInfos* CharacterTableInfo = TableManager->GetTableInfo<FTableInfos>(TableManager->DTCharacterTable, MapTableInfo->ActorIDList[i]);
				if (CharacterTableInfo)
				{
					AssetsQueue.AddUnique(CharacterTableInfo->BlueprintClass.ToSoftObjectPath());
				}
			}
		}
	}

	CachedAssetPath = AssetsQueue;


	TArray<FSoftObjectPath> TargetAssets;
	for (size_t i = 0; i < iPartition; ++i)
	{
		if (AssetsQueue.IsValidIndex(0))
		{
			FSoftObjectPath target = AssetsQueue.Pop();
			TargetAssets.AddUnique(target);
		}
	}

	if (TargetAssets.GetAllocatedSize())
		UAssetManager::Get().GetStreamableManager().RequestAsyncLoad(TargetAssets, FStreamableDelegate::CreateUObject(this, &UResourceCacheManager::Callback_AsyncLoad));
	
}

void UResourceCacheManager::Callback_AsyncLoad()
{
	TArray<FSoftObjectPath> TargetAssets;
	for (size_t i = 0; i < iPartition; ++i)
	{
		if (AssetsQueue.IsValidIndex(0))
		{
			FSoftObjectPath target = AssetsQueue.Pop();
			TargetAssets.AddUnique(target);
		}
	}
	
	if (TargetAssets.GetAllocatedSize())
	{
		LoadingWidget->ResourceText->SetText(FText::FromString(TargetAssets[0].ToString()));

		/*UUP_LoadingScene* LoadingScene = Cast<UUP_LoadingScene>(RGAMEINSTANCE(this)->LoadingSceneManager->Widget);
		if (LoadingScene->IsValidLowLevel())
		{
			LoadingScene->CurrentPercent += LoadingScene->UnitPercent;
			FMath::Clamp<int32>(LoadingScene->CurrentPercent, 0, 100);

			float percent = LoadingScene->CurrentPercent / 100.f;
			LoadingScene->SetProgressBarPercent(percent);
		}*/
		UAssetManager::Get().GetStreamableManager().RequestAsyncLoad(TargetAssets, FStreamableDelegate::CreateUObject(this, &UResourceCacheManager::Callback_AsyncLoad));
	}
	else
	{
		for (size_t i = 0; i < CachedAssetPath.Num(); ++i)
		{
			if (CachedAssetPath[i].IsValid())
			{
				if (Cast<UClass>(CachedAssetPath[i].ResolveObject()))
					CachedUClassArray.AddUnique(Cast<UClass>(CachedAssetPath[i].ResolveObject()));
				else if (CachedAssetPath[i].ResolveObject())
					CachedResourceArray.AddUnique(CachedAssetPath[i].ResolveObject());
				else
				{
					FString str = TEXT("Fail to load asset : ") + CachedAssetPath[i].GetAssetName();
					UE_LOG(LogClass, Log, TEXT("%s"), *str);
				}
			}
		}

		for (size_t i = 0; i < CachedUClassArray.Num(); ++i)
		{
			if (CachedUClassArray[i]->IsChildOf<ACharacter>())
			{
				ACharacter* Character = CachedUClassArray[i]->GetDefaultObject<ACharacter>();
				if (Character)
				{
					USkeletalMeshComponent* SkeletalMeshComp = Character->FindComponentByClass<USkeletalMeshComponent>();
					if (SkeletalMeshComp)
					{
						TSubclassOf<UAnimInstance> AnimInstance = SkeletalMeshComp->AnimClass;
						if (AnimInstance)
						{
							UE_LOG(LogClass, Log, TEXT("1"));
						}
					}
				}
			}
		}

		OnResourceCacheFinished.ExecuteIfBound((uint8)UGameStartHelper::EPrerequisiteGameStart::REMAINCACHE);
	}
}

void UResourceCacheManager::ShowLoadingScene(bool bShow)
{
	if (!IsValid(LoadingWidget))
		return;

	if (bShow)
	{
		if (!LoadingWidget->IsInViewport())
		{
			LoadingWidget->AddToViewport(9999);
		}
	}
	else
	{
		if (LoadingWidget->IsInViewport())
		{
			LoadingWidget->RemoveFromViewport();
		}
	}
}
