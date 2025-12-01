// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GAbilitySystemStatics.generated.h"

class UGameplayAbility;
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
	
	// todo: 这里用从CDO拿的，如果其他 GE修改了当前 GA 的冷却，最终显示在UI上的冷却是不会更新的
	static float GetStaticCooldownDurationForAbility(const UGameplayAbility* Ability);
	static float GetStaticCostForAbility(const UGameplayAbility* Ability);
	static FGameplayTag GetAimStatTag();
};
