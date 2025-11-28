// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GGameplayAbility.h"
#include "GAP_Launched.generated.h"

/**
 * 
 */
UCLASS()
class GREY_API UGAP_Launched : public UGGameplayAbility
{
	GENERATED_BODY()
public:
	UGAP_Launched();
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	static FGameplayTag GetLauchedAbilityActiationTag();
	
};
