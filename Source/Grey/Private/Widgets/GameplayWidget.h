// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GAS/GGameplayAbilityTypes.h"
#include "GameplayWidget.generated.h"

/**
 * 
 */
UCLASS()
class GREY_API UGameplayWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	// GameplayWidget负责管理和初始化所有的子菜单
	virtual void NativeConstruct() override;
	
	void ConfigureAbilities(const TMap<EGAbilityInputID, TSubclassOf<class UGameplayAbility>>& Abilities);
private:
	// 生命条 + 魔力条
	UPROPERTY(meta=(BindWidget))
	class UValueGauge* HealthBar;

	UPROPERTY(meta=(BindWidget))
	class UValueGauge* ManaBar;
	
	UPROPERTY(meta=(BindWidget))
	class UAbilityListView* AbilityListView;

	
	UPROPERTY()
	class UAbilitySystemComponent* OwnerAbilitySystemComponent;
};
