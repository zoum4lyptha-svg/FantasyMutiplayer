// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilityListView.h"

#include "AbilityGauge.h"
#include "Abilities/GameplayAbility.h"

void UAbilityListView::ConfigureAbilities(const TMap<EGAbilityInputID, TSubclassOf<class UGameplayAbility>>& Abilities)
{
	// 注意时序问题：AddItem下一帧才会实际向list填数据，
	// 这里直接调AbilityGaugeGenerated给icon喂数据是没有东西的，所以用委托延时触发
	OnEntryWidgetGenerated().AddUObject(this, &UAbilityListView::AbilityGaugeGenerated);

	for (const TPair<EGAbilityInputID, TSubclassOf<UGameplayAbility>>& AbilityPair : Abilities)
	{
		AddItem(AbilityPair.Value.GetDefaultObject());
		//fix: 刚 add进去的item还没生成对应的widget，所以不能在这一帧设置widget的icon数据
		//UUserWidget* NewlyCreatedGauge = GetEntryWidgetFromItem(AbilityPair.Value.GetDefaultObject())
	}
}

void UAbilityListView::AbilityGaugeGenerated(UUserWidget& Widget)
{
	UAbilityGauge* AbilityGauge = Cast<UAbilityGauge>(&Widget);

	if (AbilityGauge)
	{
		const UGameplayAbility* Ability = AbilityGauge->GetListItem<UGameplayAbility>();
		const TSubclassOf<UGameplayAbility> AbilityClass = Ability ? Ability->GetClass() : nullptr;
		const FAbilityWidgetData* WidgetData = FindWidgetDataForAbility(AbilityClass);

		AbilityGauge->ConfigureWithWidgetData(WidgetData);
	}
}

const struct FAbilityWidgetData* UAbilityListView::FindWidgetDataForAbility(
	const TSubclassOf<UGameplayAbility>& AbilityClass) const
{
	if (!AbilityDataTable)
		return nullptr;

	for (auto& AbilityWidgetDataPair : AbilityDataTable->GetRowMap())
	{
		const FAbilityWidgetData* WidgetData = AbilityDataTable->FindRow<FAbilityWidgetData>(AbilityWidgetDataPair.Key, "");
		// 匹配 data table 这一行有没有你要的 GA
		if (WidgetData->AbilityClass == AbilityClass)
		{
			return WidgetData;
		}
	}

	return nullptr;
}
