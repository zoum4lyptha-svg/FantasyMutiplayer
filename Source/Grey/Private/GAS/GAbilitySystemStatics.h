// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GAbilitySystemStatics.generated.h"

/**
 * 
 */
UCLASS()
class GREY_API UGAbilitySystemStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	static FGameplayTag GetBasicAttackAbilityTag();

	static FGameplayTag GetDeadStatTag();

	static FGameplayTag GetStunStatTag();

	static FGameplayTag GetBasicAttackInputPressedTag();
};
