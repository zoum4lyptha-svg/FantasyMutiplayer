// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GGameplayAbility.h"
#include "GAS/GGameplayAbilityTypes.h"
#include "GA_UpperCut.generated.h"

/**
 * 
 */
UCLASS()
class GREY_API UGA_UpperCut : public UGGameplayAbility
{
	GENERATED_BODY()

public:

	UGA_UpperCut();
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

private:
	
	UPROPERTY(EditDefaultsOnly, Category = "Combo")
	TMap<FName, FGenericDamgeEffectDef> ComboDamageMap;
	
	UPROPERTY(EditDefaultsOnly, Category = "Launch")
	TSubclassOf<UGameplayEffect> LaunchDamageEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Launch")
	float UpperCutLaunchSpeed = 1000.f;

	UPROPERTY(EditDefaultsOnly, Category = "Launch")
	float UpperComboHoldSpeed= 100.f;
	

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* UpperCutMontage;

	static FGameplayTag GetUpperCutLaunchTag();

	const FGenericDamgeEffectDef* GetDamageEffectDefForCurrentCombo() const;
	
	UFUNCTION()
	void StartLaunching(FGameplayEventData EventData);

	UFUNCTION()
	void HandleComboCommitEvent(FGameplayEventData EventData);

	UFUNCTION()
	void HandleComboDamageEvent(FGameplayEventData EventData);

	UFUNCTION()
	void HandleComboChangeEvent(FGameplayEventData EventData);

	FName NextComboName;
};

