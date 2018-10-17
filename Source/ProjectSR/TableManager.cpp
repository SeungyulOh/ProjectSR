// Fill out your copyright notice in the Description page of Project Settings.

#include "TableManager.h"
#include "ProjectSR.h"




void UTableManager::Init()
{
	DTCharacterTable = LoadTableFromPath(TEXT("DataTable'/Game/Data/DT_CharacterTable.DT_CharacterTable'"));
	DTMapTable = LoadTableFromPath(TEXT("DataTable'/Game/Data/DT_MapTable.DT_MapTable'"));
	DTWidgetTable = LoadTableFromPath(TEXT("DataTable'/Game/Data/DT_WidgetTable.DT_WidgetTable'"));
}

UDataTable* UTableManager::LoadTableFromPath(const FName& Path)
{
	if (Path == NAME_None) return nullptr;

	UObject* DataTableObject = (StaticLoadObject(UDataTable::StaticClass(), nullptr, *Path.ToString()));
	UDataTable* DataTablePtr = Cast< UDataTable >(DataTableObject);

	return DataTablePtr;
}

