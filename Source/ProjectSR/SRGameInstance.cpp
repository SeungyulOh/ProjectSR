// Fill out your copyright notice in the Description page of Project Settings.

#include "SRGameInstance.h"
#include "ProjectSR.h"
#include "TableManager.h"
#include "ResourceCacheManager.h"

PROJECTSR_API USRGameInstance* SRGameInstance = nullptr;

void USRGameInstance::Init()
{
	Super::Init();

	SRGameInstance = this;
	
	/*Initialization*/
	TableManager->Init();
	GameStartHelper->Init();
}

void USRGameInstance::Shutdown()
{
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

}
