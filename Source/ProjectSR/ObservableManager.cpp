// Fill out your copyright notice in the Description page of Project Settings.

#include "ObservableManager.h"
#include "Observable_StageData.h"

UObservableManager* UObservableManager::InstancePtr = nullptr;

void UObservableManager::Init()
{
	UObservable_StageData* StageData = NewObject<UObservable_StageData>();
	if (StageData)
	{
		StageData->AddToRoot();
		ObservableArray.Emplace(StageData);
	}
}

void UObservableManager::Reset()
{
	for (class IObservable* Element : ObservableArray)
	{
		UObject* obj = Cast<UObject>(Element);
		if (IsValid(obj))
			obj->RemoveFromRoot();
	}
}

UObservableManager* UObservableManager::Get()
{
	if (InstancePtr == nullptr)
	{
		InstancePtr = NewObject<UObservableManager>();
		InstancePtr->Init();
		InstancePtr->AddToRoot();
	}

	return InstancePtr;
}

void UObservableManager::ReleaseSingleton()
{
	if (InstancePtr)
	{
		InstancePtr->Reset();
		InstancePtr->RemoveFromRoot();
		InstancePtr = nullptr;
	}
}
