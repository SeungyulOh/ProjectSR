// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#define WIDGET_LOADING FName(TEXT("UP_LoadingScene"))
#define WIDGET_INGAME  FName(TEXT("UP_Ingame"))
#define WIDGET_MESSAGE FName(TEXT("UP_MessageNotifier"))


UENUM(BlueprintType)
enum class EUserModeEnum : uint8
{
	ENORMAL						UMETA(DisplayName = "ENORMAL"),
	ETOPVIEW					UMETA(DisplayName = "ETOPVIEW"),
	EBUILDING_IDLE				UMETA(DisplayName = "EBUILDING_IDLE"),
	EBUILDING_ADDSTART			UMETA(DisplayName = "EBUILDING_ADDSTART"),
	EBUILDING_ADDING			UMETA(DisplayName = "EBUILDING_ADDING"),
	EBUILDING_WALLSELECTING		UMETA(DisplayName = "EBUILDING_WALLSELECTING"),
	EBUILDING_END				UMETA(DisplayName = "EBUILDING_END"),
	EEND						UMETA(DisplayName = "EEND")
};