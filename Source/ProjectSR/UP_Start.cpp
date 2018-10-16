// Fill out your copyright notice in the Description page of Project Settings.

#include "UP_Start.h"
#include "ProjectSR.h"



void UUP_Start::NativeConstruct()
{
	Super::NativeConstruct();

	/*Button Settings*/
	if (Button_Start)
	{
		Button_Start->OnClicked.Clear();
		Button_Start->OnClicked.AddDynamic(this, &UUP_Start::Callback_ButtonStart);
	}
	/**/
}

void UUP_Start::NativeDestruct()
{
	Super::NativeDestruct();
}

void UUP_Start::Callback_ButtonStart()
{
	SRGAMEINSTANCE(this)->GameStartHelper->Decide_NextAction();

	//UPackage* package = SRGAMEINSTANCE(this)->MapArray[0];
	//if (IsValid(package) && package->IsFullyLoaded())
	//{
	//	FString path = package->GetName();
	//	float LoadedTime = package->GetLoadTime();
	//	
	//	/*
	//	Make Sure put Corrent Map Name, not path!
	//	Why? If you put Path instead of Name , your character won't work as intended.
	//	*/

	//	UGameplayStatics::OpenLevel(this, TEXT("OutLand_Field_01_P"));
	//}

	/*UGameplayStatics::OpenLevel(this, TEXT("OutLand_Field_01_P"));*/
}
