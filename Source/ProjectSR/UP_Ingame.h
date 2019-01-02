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
	class UButton*	Button_Remove;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FVariables")
	class UButton*	Button_Complete;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FVariables")
	class UButton*	Button_Prev;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FVariables")
	class UTextBlock*	BuildingButtonText;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FVariables")
	class UTextBlock*	CamButtonText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FVariables")
	class UUC_SkillSelector*	SkillSelector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FVariables")
	class UUC_Topbar*	Topbar;


	

};

USTRUCT(BlueprintType)
struct PROJECTSR_API FRenderer
{
	GENERATED_USTRUCT_BODY()

	void Prepare();
	void Render();

	void Reposition_SubUI();

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
	UFUNCTION(BlueprintCallable)
	void OnClick_ButtonRemove();
	UFUNCTION(BlueprintCallable)
	void OnClick_ButtonComplete();
	UFUNCTION(BlueprintCallable)
	void OnClick_ButtonPrev();


public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UP_Ingame")
	FVariables Variables;

	UPROPERTY()
	FRenderer Renderer;
};
