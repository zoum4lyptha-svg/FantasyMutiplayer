// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "MMC_LevelBased.generated.h"

/**
 * 
 */
UCLASS()
class GREY_API UMMC_LevelBased : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()
	
public:
	UMMC_LevelBased();
	// 基于等级/成长率 计算升级获得的智力/力量
	float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
private:
	
	UPROPERTY(EditDefaultsOnly)
	FGameplayAttribute RateAttribute;

	FGameplayEffectAttributeCaptureDefinition LevelCaptureDefination;
};
