// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "GameplayEffectTypes.h"
#include "Blueprint/UserWidget.h"
#include "AbilityGauge.generated.h"



class UAbilitySystemComponent;
struct FGameplayAbilitySpec;
/**
 *   虚幻神人设计之不带 IUserObjectListEntry 这个接口是不给你配 UListView表的
 */

USTRUCT(BlueprintType)
struct FAbilityWidgetData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class UGameplayAbility> AbilityClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName AbilityName;

	// 这里纹理用TSoftObjectPtr，用到时才会加载，节约内存
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UTexture2D> Icon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Description;
};

UCLASS()
class GREY_API UAbilityGauge : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;
	// 这个会在标记为IUserObjectListEntry的item 添加进list（additem）执行结束后调用
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
	
	void ConfigureWithWidgetData(const FAbilityWidgetData* WidgetData);
private:
	UPROPERTY(EditDefaultsOnly, Category = "Visual")
	FName IconMaterialParamName = "Icon";
	
	UPROPERTY(EditDefaultsOnly, Category = "Cooldown")
	float CooldownUpdateInterval = 0.1f;
	
	
	UPROPERTY(meta=(BindWidget))
	class UImage* Icon;

	// material param
	UPROPERTY(EditDefaultsOnly, Category = "Visual")
	FName CooldownPercentParamname = "Percent";
	
	// material param for level up
	UPROPERTY(EditDefaultsOnly, Category = "Visual")
	FName AbilityLevelParamName = "Level";

	UPROPERTY(EditDefaultsOnly, Category = "Visual")
	FName CanCastAbilityParamName = "CanCast";
	
	UPROPERTY(EditDefaultsOnly, Category = "Visual")
	FName UpgradePointAvaliableParamName = "UpgradeAvaliable";
	
	
	UPROPERTY(meta=(BindWidget))
	class UTextBlock* CooldownCounterText;

	UPROPERTY(meta=(BindWidget))
	class UTextBlock* CooldownDurationText;

	UPROPERTY(meta=(BindWidget))
	class UTextBlock* CostText;
	
	
	UPROPERTY(meta=(BindWidget))
	class UImage* LevelGauge;
	
	
	UPROPERTY()
	class UGameplayAbility* AbilityCDO;
	
	
	void AbilityCommitted(UGameplayAbility* Ability);

	void StartCooldown(float CooldownTimeRemaining, float CooldownDuration);

	float CachedCooldownDuration;
	float CachedCooldownTimeRemaining;

	FTimerHandle CooldownTimerHandle;
	FTimerHandle CooldownTimerUpdateHandle;

	FNumberFormattingOptions WholeNumberFormattionOptions;
	FNumberFormattingOptions TwoDigitNumberFormattingOptions;

	void CooldownFinished();
	void UpdateCooldown();
	
	
	const UAbilitySystemComponent* OwnerAbilitySystemComponent;
	const FGameplayAbilitySpec* CachedAbilitySpec;

	const FGameplayAbilitySpec* GetAbilitySpec();

	bool bIsAbilityLeanred = false;

	void AbilitySpecUpdated(const FGameplayAbilitySpec& AbilitySpec);
	void UpdateCanCast();
	void UpgradePointUpdated(const FOnAttributeChangeData& Data);
	
	void ManaUpdated(const FOnAttributeChangeData& Data);
};
