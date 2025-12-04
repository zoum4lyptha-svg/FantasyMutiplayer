// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "MMC_BaseAttackDamage.generated.h"

/**
 * 
 */
UCLASS()
class GREY_API UMMC_BaseAttackDamage : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()
	
public:
	UMMC_BaseAttackDamage();
	// 结合 双方的攻击/护甲 计算最终伤害
	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
private:
	FGameplayEffectAttributeCaptureDefinition DamageCaptureDef;
	FGameplayEffectAttributeCaptureDefinition ArmorCaptureDef;
};
