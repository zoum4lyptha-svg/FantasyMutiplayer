// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class GREY_API UGGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
	//主要为GA子类提供一些方法
protected:
	class UAnimInstance* GetOwnerAnimInstance() const;
};
