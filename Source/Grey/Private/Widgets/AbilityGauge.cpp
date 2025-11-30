// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilityGauge.h"

#include "Abilities/GameplayAbility.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "GAS/GAbilitySystemStatics.h"

void UAbilityGauge::NativeConstruct()
{
	CooldownCounterText->SetVisibility(ESlateVisibility::Hidden);
}

void UAbilityGauge::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	// AbilityListView add item 给表中新加 item后，从GA的CDO中拿data配置item
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);
	AbilityCDO = Cast<UGameplayAbility>(ListItemObject);

	float CooldownDuration = UGAbilitySystemStatics::GetStaticCooldownDurationForAbility(AbilityCDO);
	float Cost = UGAbilitySystemStatics::GetStaticCostForAbility(AbilityCDO);

	CooldownDurationText->SetText(FText::AsNumber(CooldownDuration));
	CostText->SetText(FText::AsNumber(Cost));
}

void UAbilityGauge::ConfigureWithWidgetData(const FAbilityWidgetData* WidgetData)
{
	if (Icon && WidgetData)
	{
		// 给 UMG 的 icon 组件传纹理
		Icon->GetDynamicMaterial()->SetTextureParameterValue(IconMaterialParamName, WidgetData->Icon.LoadSynchronous());
	}
}
