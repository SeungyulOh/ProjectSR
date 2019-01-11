// Fill out your copyright notice in the Description page of Project Settings.

#include "Observable_StageData.h"




void UObservable_StageData::Register(ISRObserver* InObserver)
{
	UObject* obj = Cast<UObject>(InObserver);
	if (IsValid(obj))
	{
		obj->AddToRoot();
		ObserverArray.AddUnique(InObserver);
	}
	
}

void UObservable_StageData::UnRegister(ISRObserver* InObserver)
{
	int32 FoundIdx = ObserverArray.Find(InObserver);
	if (FoundIdx != INDEX_NONE)
	{
		UObject* obj = Cast<UObject>(InObserver);
		if (obj)
			obj->RemoveFromRoot();

		ObserverArray.RemoveAt(FoundIdx);
	}
}

void UObservable_StageData::Notify()
{
	for (auto& Element : ObserverArray)
		Element->Update();
}

