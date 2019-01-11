// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Observable.h"
#include "ScriptInterface.h"
#include "Observable_StageData.generated.h"

/**
 * 
 */

USTRUCT(BlueprintType)
struct PROJECTSR_API FStageData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	int32 CurrentStage = 1;
	UPROPERTY()
	int32 CurrentGold = 1000;
	UPROPERTY()
	int32 MonsterRemaining = 0;
};


UCLASS()
class PROJECTSR_API UObservable_StageData : public UObject, public IObservable
{
	GENERATED_BODY()

	friend class AStageGameMode;
	
public:
	virtual void Register(ISRObserver* InObserver);
	virtual void UnRegister(ISRObserver* InObserver);

public:
	FORCEINLINE const FStageData& GetStageData() { return StageData; }

protected:
	virtual void Notify();

private:
	TArray<ISRObserver*> ObserverArray;

	UPROPERTY()
	FStageData	StageData;

};


