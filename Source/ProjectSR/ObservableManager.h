// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ObservableManager.generated.h"

UENUM(BlueprintType)
enum class EObservableType : uint8
{
	STAGEDATA,
	END
};

/**
 * 
 */
UCLASS()
class PROJECTSR_API UObservableManager : public UObject
{
	GENERATED_BODY()

public:
	void Init();
	void Reset();

private:
	TArray<class IObservable*> ObservableArray;



public:
	static UObservableManager* Get();
	static void ReleaseSingleton();


private:
	UObservableManager() {};

	static UObservableManager* InstancePtr;


public:
	template<typename T>
	T* GetObservable()
	{
		for (class IObservable* Element : ObservableArray)
		{
			T* CastedInterface = (T*)Element;
			if (CastedInterface)
				return CastedInterface;
		}

		return nullptr;
	}
	
};
