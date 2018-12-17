// Fill out your copyright notice in the Description page of Project Settings.

#include "TableManager.h"
#include "ProjectSR.h"




void UTableManager::Init()
{
	DTObjectTable = LoadTableFromPath(TEXT("DataTable'/Game/Data/DT_ObjectTable.DT_ObjectTable'"));
	DTMapTable = LoadTableFromPath(TEXT("DataTable'/Game/Data/DT_MapTable.DT_MapTable'"));
	DTWidgetTable = LoadTableFromPath(TEXT("DataTable'/Game/Data/DT_WidgetTable.DT_WidgetTable'"));
	DTResourceCacheTable = LoadTableFromPath(TEXT("DataTable'/Game/Data/DT_ResourceCacheTable.DT_ResourceCacheTable'"));
}

UDataTable* UTableManager::LoadTableFromPath(const FName& Path)
{
	if (Path == NAME_None) return nullptr;

	UObject* DataTableObject = (StaticLoadObject(UDataTable::StaticClass(), nullptr, *Path.ToString()));
	UDataTable* DataTablePtr = Cast< UDataTable >(DataTableObject);

	return DataTablePtr;
}

