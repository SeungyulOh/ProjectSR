// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "TableInfos.generated.h"
/**
*
*/
USTRUCT()
struct FTableInfos : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TableInfos")
	TSoftClassPtr<class AActor> BlueprintClass;

};

USTRUCT()
struct FMapTableInfos : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TableInfos")
	TSoftObjectPtr<class UWorld> MapObjectPtr;

};