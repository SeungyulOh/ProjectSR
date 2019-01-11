// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SRObserver.h"
#include "Observable.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UObservable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PROJECTSR_API IObservable
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void Register(ISRObserver* InObserver) = 0;
	virtual void UnRegister(ISRObserver* InObserver) = 0;

protected:
	virtual void Notify() = 0;
	
};

