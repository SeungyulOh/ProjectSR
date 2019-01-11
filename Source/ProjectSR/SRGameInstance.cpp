// Fill out your copyright notice in the Description page of Project Settings.

#include "SRGameInstance.h"
#include "ProjectSR.h"
#include "TableManager.h"
#include "ResourceCacheManager.h"
#include "ObservableManager.h"

PROJECTSR_API USRGameInstance* SRGameInstance = nullptr;

void USRGameInstance::Init()
{
	Super::Init();

	SRGameInstance = this;
	
	/*Initialization*/
	TableManager->Init();
	GameStartHelper->Init();
	ResourceCacheManager->Init();
}

void USRGameInstance::Shutdown()
{
	UObservableManager::ReleaseSingleton();

	Super::Shutdown();
}

USRGameInstance::USRGameInstance(const FObjectInitializer& ObjectInitializer) 
	:Super(ObjectInitializer)
{
	GameStartHelper = CreateDefaultSubobject<UGameStartHelper>(TEXT("GameHelper"));
	TableManager = CreateDefaultSubobject<UTableManager>(TEXT("TableManager"));
	ResourceCacheManager = CreateDefaultSubobject<UResourceCacheManager>(TEXT("ResourceManager"));
}

class USRGameInstance* USRGameInstance::GetSRGameInstance(class UObject* OutterOwner)
{
#ifdef WITH_EDITOR
	if (OutterOwner == nullptr || OutterOwner->GetWorld() == nullptr)
	{
		return SRGameInstance;
	}

	USRGameInstance* GameInstance = Cast<USRGameInstance>(OutterOwner->GetWorld()->GetGameInstance());
	if (GameInstance == nullptr)
	{
		ensure(GameInstance == nullptr);
		return SRGameInstance;
	}
	return GameInstance;
#else
	return SRGameInstance;
#endif
}

void UGameStartHelper::Init()
{
	RequirementArray.Init(false, (int32)EPrerequisiteGameStart::END);

	USRGameInstance* GameInst = Cast<USRGameInstance>(GetOuter());
	if (GameInst)
	{
		GameInst->ResourceCacheManager->OnResourceCacheFinished.Unbind();
		GameInst->ResourceCacheManager->OnResourceCacheFinished.BindUObject(this, &UGameStartHelper::Callback_ResourceCacheFinished);
	}
	
}

void UGameStartHelper::Decide_NextAction()
{
	if (RequirementArray[(int32)EPrerequisiteGameStart::MAPCACHE] == 0)
	{
		Do_CacheMap();
		return;
	}
	else if (RequirementArray[(int32)EPrerequisiteGameStart::REMAINCACHE] == 0)
	{
		Do_CacheRemains();
		return;
	}

	Do_StartGame();
}

void UGameStartHelper::Callback_ResourceCacheFinished(uint8 Sequence)
{
	if (RequirementArray.IsValidIndex(Sequence))
	{
		RequirementArray[Sequence] = true;
	}

	Decide_NextAction();
}


void UGameStartHelper::Do_CacheMap()
{
	USRGameInstance* GameInst = Cast<USRGameInstance>(GetOuter());
	if (GameInst)
	{
		GameInst->ResourceCacheManager->Cache_Map();
	}
}

void UGameStartHelper::Do_CacheRemains()
{
	USRGameInstance* GameInst = Cast<USRGameInstance>(GetOuter());
	if (GameInst)
	{
		GameInst->ResourceCacheManager->Cache_Remains();
	}
}

void UGameStartHelper::Do_StartGame()
{
	USRGameInstance* GameInst = Cast<USRGameInstance>(GetOuter());
	UTableManager* TableManager = GameInst->TableManager;
	UResourceCacheManager* ResourceCacheManager = GameInst->ResourceCacheManager;

	FMapTableInfos* MapTableInfo = TableManager->GetTableInfo<FMapTableInfos>(TableManager->DTMapTable, FName(*FString::FromInt(CurrentStage)));
	if (MapTableInfo)
	{
		UGameplayStatics::OpenLevel(SRGAMEINSTANCE(GEngine), MapTableInfo->MapName);
		return;
		if (IsValid(ResourceCacheManager->CachedMap) && ResourceCacheManager->CachedMap->IsFullyLoaded())
		{
			/*
			Make Sure put Corrent Map Name, not path!
			Why? If you put Path instead of Name , your character won't work as intended.
			*/
			bool isMapPackage = ResourceCacheManager->CachedMap->ContainsMap();
			FString mapStr = ResourceCacheManager->CachedMap->FileName.ToString();
			bool isvalidMapName = GEngine->MakeSureMapNameIsValid(mapStr);
			UGameplayStatics::OpenLevel(SRGAMEINSTANCE(GEngine), ResourceCacheManager->CachedMap->FileName);

		}
	}
	
}
