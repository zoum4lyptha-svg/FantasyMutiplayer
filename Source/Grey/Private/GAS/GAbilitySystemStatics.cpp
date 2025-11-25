// Fill out your copyright notice in the Description page of Project Settings.


#include "GAbilitySystemStatics.h"

FGameplayTag UGAbilitySystemStatics::GetBasicAttackAbilityTag()
{
	return FGameplayTag::RequestGameplayTag("ability.basicattack");
}

FGameplayTag UGAbilitySystemStatics::GetDeadStatTag()
{
	return FGameplayTag::RequestGameplayTag("stats.dead");
}
