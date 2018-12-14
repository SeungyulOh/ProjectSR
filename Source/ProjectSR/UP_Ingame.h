// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UP_Ingame.generated.h"


USTRUCT(BlueprintType)
struct PROJECTSR_API FVariables
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere , BlueprintReadWrite , Category = "FVariables")
	class UOverlay* SubUIOverlay;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FVariables")
	class UButton*	Button_CamMode;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FVariables")
	class UButton*	Button_BuildingMode;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FVariables")
	class UButton*	Button_GameStartMode;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FVariables")
	class UButton*	Button_Firstspawn;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FVariables")
	class UButton*	Button_Confirm;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FVariables")
	class UTextBlock*	BuildingButtonText;

};

USTRUCT(BlueprintType)
struct PROJECTSR_API FRenderer
{
	GENERATED_USTRUCT_BODY()

	void Prepare();
	void Render();

	FVariables* variables;
};


UCLASS()
class PROJECTSR_API UUP_Ingame : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void NativeConstruct() override;
	void NativeDestruct() override;

	UFUNCTION(BlueprintCallable)
	void OnClick_CameraToggle();
	UFUNCTION(BlueprintCallable)
	void OnClick_ButtonReady();
	UFUNCTION(BlueprintCallable)
	void OnClick_ButtonBuilding();
	UFUNCTION(BlueprintCallable)
	void OnClick_ButtonFirstSpawn();
	UFUNCTION(BlueprintCallable)
	void OnClick_ButtonConfirm();

	UFUNCTION()
	void Callback_ClickedWhenBuildingMode(FVector ViewportLocation);

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UP_Ingame")
	FVariables Variables;

	UPROPERTY()
	FRenderer Renderer;
};
