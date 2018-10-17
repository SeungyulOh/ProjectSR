// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/DataTable.h"
#include "TableInfos.h"
#include "TableManager.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTSR_API UTableManager : public UObject
{
	GENERATED_BODY()
	
public:
	void Init();
	UDataTable* LoadTableFromPath(const FName& Path);

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = TableManager)
	UDataTable* DTCharacterTable;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = TableManager)
	UDataTable* DTMapTable;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = TableManager)
	UDataTable* DTWidgetTable;


	template<typename T>
	T* GetTableInfo(UDataTable* DataTable, FName TableKey)
	{
		T* RowInfo = DataTable->FindRow<T>(TableKey, TEXT("GENERAL"), 0);
		return RowInfo;
	}
	
};
