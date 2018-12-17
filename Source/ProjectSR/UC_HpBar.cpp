// Fill out your copyright notice in the Description page of Project Settings.

#include "UC_HpBar.h"
#include "ProjectSR.h"




void UUC_HpBar::SetHPPercent(float hp)
{
	if (IsValid(HPProgressBar))
	{
		HPProgressBar->SetPercent(hp);
	}
}
