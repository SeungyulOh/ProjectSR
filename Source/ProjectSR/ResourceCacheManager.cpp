// Fill out your copyright notice in the Description page of Project Settings.

#include "ResourceCacheManager.h"
#include "ProjectSR.h"
#include "TableManager.h"

void UResourceCacheManager::Cache_Map()
{
	USRGameInstance* GameInst = Cast<USRGameInstance>(GetOuter());
	if (GameInst)
	{
		int32 CurrentStage = GameInst->GameStartHelper->CurrentStage;

		UTableManager* TableManager = GameInst->TableManager;
		FMapTableInfos* MapTableInfo = TableManager->GetTableInfo<FMapTableInfos>(TableManager->DTMapTable, FName(*FString::FromInt(CurrentStage)));
		if (MapTableInfo)
		{
			FSoftObjectPath path = MapTableInfo->MapObjectPtr.ToSoftObjectPath();
			FSoftObjectPath path2 = MapTableInfo->MapObjectPtr.ToSoftObjectPath();
		}
	}

	

	FSoftObjectPath path;
	path.SetPath(TEXT("/Game/Maps/Campaign/OutLand/OutLand_Field_01/OutLand_Field_01_P.OutLand_Field_01_P"));

	FLoadPackageAsyncDelegate Delegates;
	Delegates.BindLambda([this](const FName& PackageName, UPackage* LoadedPackage, EAsyncLoadingResult::Type Result) {
		if (Result == EAsyncLoadingResult::Succeeded)
		{
			UE_LOG(LogClass, Log, TEXT("%s"), *PackageName.ToString());
			MapArray.AddUnique(LoadedPackage);
		}
	});
	LoadPackageAsync(path.GetLongPackageName(), Delegates);
}

void UResourceCacheManager::Cache_Remains()
{

}
