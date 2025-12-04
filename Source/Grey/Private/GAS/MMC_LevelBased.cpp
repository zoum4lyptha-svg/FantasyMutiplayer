// Fill out your copyright notice in the Description page of Project Settings.


#include "MMC_LevelBased.h"

#include "GHeroAttributeSet.h"

UMMC_LevelBased::UMMC_LevelBased()
{
	LevelCaptureDefination.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	LevelCaptureDefination.AttributeToCapture = UGHeroAttributeSet::GetLevelAttribute();

	RelevantAttributesToCapture.Add(LevelCaptureDefination);
}

float UMMC_LevelBased::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	UAbilitySystemComponent* ASC = Spec.GetContext().GetInstigatorAbilitySystemComponent();
	if (!ASC)
		return 0.f;

	float Level = 0;
	FAggregatorEvaluateParameters EvalParams;
	EvalParams.SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	EvalParams.TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
	// capture level
	GetCapturedAttributeMagnitude(LevelCaptureDefination, Spec, EvalParams, Level);

	bool bFound;
	//  rate不会动态修改，这里直接拿值了，没拿快照
	float RateAttributeVal = ASC->GetGameplayAttributeValue(RateAttribute, bFound);
	if (!bFound)
		return 0.f;

	return (Level - 1) * RateAttributeVal;
}
