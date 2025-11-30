// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilityListView.h"

#include "Abilities/GameplayAbility.h"

void UAbilityListView::ConfigureAbilities(const TMap<EGAbilityInputID, TSubclassOf<class UGameplayAbility>>& Abilities)
{
	for (const TPair<EGAbilityInputID, TSubclassOf<UGameplayAbility>>& AbilityPair : Abilities)
	{
		AddItem(AbilityPair.Value.GetDefaultObject());
	}
}
