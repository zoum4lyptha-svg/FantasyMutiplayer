// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"

#include "GAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

/**
 * 
 */
UCLASS()
class GREY_API UGAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

	// 属性 getter setter
public:
	ATTRIBUTE_ACCESSORS(UGAttributeSet, Health)
	ATTRIBUTE_ACCESSORS(UGAttributeSet, MaxHealth)
	ATTRIBUTE_ACCESSORS(UGAttributeSet, Mana)
	ATTRIBUTE_ACCESSORS(UGAttributeSet, MaxMana)
private:
	UPROPERTY()
	FGameplayAttributeData Health;

	UPROPERTY()
	FGameplayAttributeData MaxHealth;

	UPROPERTY()
	FGameplayAttributeData Mana;
	
	UPROPERTY()
	FGameplayAttributeData MaxMana;
};
