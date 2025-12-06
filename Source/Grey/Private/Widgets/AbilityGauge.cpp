// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilityGauge.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Abilities/GameplayAbility.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "GAS/GAbilitySystemStatics.h"
#include "GAS/GAttributeSet.h"
#include "GAS/GHeroAttributeSet.h"

void UAbilityGauge::NativeConstruct()
{
	Super::NativeConstruct();
	CooldownCounterText->SetVisibility(ESlateVisibility::Hidden);
	UAbilitySystemComponent* OwnerASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwningPlayerPawn());
	if (OwnerASC)
	{
		// 注意这里监听的是GA的实际Commit，不是activate,因为只有在实际CommitAbility应用GA后才会正式进入冷却。而activate是可以随时取消的
		OwnerASC->AbilityCommittedCallbacks.AddUObject(this, &UAbilityGauge::AbilityCommitted);
		// 监听GA spec的变化 更新材质的level,canCast等（等级，是否显示灰色）
		OwnerASC->AbilitySpecDirtiedCallbacks.AddUObject(this, &UAbilityGauge::AbilitySpecUpdated);
		// 监听 升级点变化，更新材质的UpgradePoint(level材质点亮第几个icon)
		OwnerASC->GetGameplayAttributeValueChangeDelegate(UGHeroAttributeSet::GetUpgradePointAttribute()).AddUObject(this, &UAbilityGauge::UpgradePointUpdated);
	
		OwnerASC->GetGameplayAttributeValueChangeDelegate(UGAttributeSet::GetManaAttribute()).AddUObject(this, &UAbilityGauge::ManaUpdated);
	
		bool bFound = false;
		float UpgradePoint = OwnerASC->GetGameplayAttributeValue(UGHeroAttributeSet::GetUpgradePointAttribute(), bFound);
		if (bFound)
		{
			FOnAttributeChangeData ChangeData;
			ChangeData.NewValue = UpgradePoint;
			UpgradePointUpdated(ChangeData);
		}
	}
	

	OwnerAbilitySystemComponent = OwnerASC;
	WholeNumberFormattionOptions.MaximumFractionalDigits = 0;
	TwoDigitNumberFormattingOptions.MaximumFractionalDigits = 2;
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
	// fix: 防呆兜底，材质中level默认可能不是0
	LevelGauge->GetDynamicMaterial()->SetScalarParameterValue(AbilityLevelParamName, 0);
}

void UAbilityGauge::ConfigureWithWidgetData(const FAbilityWidgetData* WidgetData)
{
	if (Icon && WidgetData)
	{
		// 给 UMG 的 icon 组件传纹理
		Icon->GetDynamicMaterial()->SetTextureParameterValue(IconMaterialParamName, WidgetData->Icon.LoadSynchronous());
	}
}

void UAbilityGauge::AbilityCommitted(UGameplayAbility* Ability)
{
	
	if (Ability->GetClass()->GetDefaultObject() == AbilityCDO)
	{
		float CooldownTimeRemaining = 0.f;
		float CooldownDuration = 0.f;

		// todo：这里有个问题，虽然拿到的是目标GA激活时实际的冷却和剩余时间。但是在本地计时期间 GA的冷却被其他GE修改了，
		// UI是不会刷新的！  最好还是用 lyra 那套监听GE委托的方案
		Ability->GetCooldownTimeRemainingAndDuration(Ability->GetCurrentAbilitySpecHandle(), Ability->GetCurrentActorInfo(), CooldownTimeRemaining, CooldownDuration);

		StartCooldown(CooldownTimeRemaining, CooldownDuration);
	}
}

void UAbilityGauge::StartCooldown(float CooldownTimeRemaining, float CooldownDuration)
{
	CooldownDurationText->SetText(FText::AsNumber(CooldownDuration));
	CachedCooldownDuration = CooldownDuration;
	CachedCooldownTimeRemaining = CooldownTimeRemaining;

	CooldownCounterText->SetVisibility(ESlateVisibility::Visible);

	// 冷却计时
	GetWorld()->GetTimerManager().SetTimer(CooldownTimerHandle, this, &UAbilityGauge::CooldownFinished, CachedCooldownTimeRemaining);
	// 更新 UI频率计时
	GetWorld()->GetTimerManager().SetTimer(CooldownTimerUpdateHandle, this, &UAbilityGauge::UpdateCooldown, CooldownUpdateInterval, true, 0.f);
	
	Icon->GetDynamicMaterial()->SetScalarParameterValue(CooldownPercentParamname, 1.f);

}

void UAbilityGauge::CooldownFinished()
{
	CachedCooldownDuration = CachedCooldownTimeRemaining = 0.f;
	CooldownCounterText->SetVisibility(ESlateVisibility::Hidden);
	GetWorld()->GetTimerManager().ClearTimer(CooldownTimerUpdateHandle);
}

void UAbilityGauge::UpdateCooldown()
{
	CachedCooldownTimeRemaining -= CooldownUpdateInterval;
	FNumberFormattingOptions* FormattingOptions = CachedCooldownTimeRemaining > 1 ? &WholeNumberFormattionOptions : &TwoDigitNumberFormattingOptions;
	CooldownCounterText->SetText(FText::AsNumber(CachedCooldownTimeRemaining, FormattingOptions));
	
	Icon->GetDynamicMaterial()->SetScalarParameterValue(CooldownPercentParamname, 1.0f - CachedCooldownTimeRemaining / CachedCooldownDuration);
}

const FGameplayAbilitySpec* UAbilityGauge::GetAbilitySpec()
{
	// 只拿一次就够了，GA 的 spec是全局唯一的
	if (!CachedAbilitySpec)
	{
		if (AbilityCDO && OwnerAbilitySystemComponent)
		{
			CachedAbilitySpec = OwnerAbilitySystemComponent->FindAbilitySpecFromClass(AbilityCDO->GetClass());
		}
	}

	return CachedAbilitySpec;
}

void UAbilityGauge::AbilitySpecUpdated(const FGameplayAbilitySpec& AbilitySpec)
{
	if (AbilitySpec.Ability != AbilityCDO)
		return;

	bIsAbilityLeanred = AbilitySpec.Level > 0;
	LevelGauge->GetDynamicMaterial()->SetScalarParameterValue(AbilityLevelParamName, AbilitySpec.Level);
	UpdateCanCast();
	
	float NewCooldownDuration = UGAbilitySystemStatics::GetCooldownDurationFor(AbilitySpec.Ability, *OwnerAbilitySystemComponent, AbilitySpec.Level);
	float NewCost = UGAbilitySystemStatics::GetManaCostFor(AbilitySpec.Ability, *OwnerAbilitySystemComponent, AbilitySpec.Level);
	CooldownDurationText->SetText(FText::AsNumber(NewCooldownDuration));
	CostText->SetText(FText::AsNumber(NewCost));
}

void UAbilityGauge::UpdateCanCast()
{
	const FGameplayAbilitySpec* AbilitySpec = GetAbilitySpec();
	bool bCanCast = bIsAbilityLeanred;
	// bCanCast 1.bIsAbilityLeanred 2.cost检查通过
	if (AbilitySpec)
	{
		if (OwnerAbilitySystemComponent && !UGAbilitySystemStatics::CheckAbilityCost(*AbilitySpec, *OwnerAbilitySystemComponent))
		{
			bCanCast = false;
		}
	}

	Icon->GetDynamicMaterial()->SetScalarParameterValue(CanCastAbilityParamName, bCanCast ? 1 : 0);
}

void UAbilityGauge::UpgradePointUpdated(const FOnAttributeChangeData& Data)
{
	
	bool HasUpgradePoint = Data.NewValue > 0;
	const FGameplayAbilitySpec* AbilitySpec = GetAbilitySpec();
	if (AbilitySpec)
	{
		if (UGAbilitySystemStatics::IsAbilityAtMaxLevel(*AbilitySpec))
		{
			Icon->GetDynamicMaterial()->SetScalarParameterValue(UpgradePointAvaliableParamName, 0);
			return;
		}
	}
	Icon->GetDynamicMaterial()->SetScalarParameterValue(UpgradePointAvaliableParamName, HasUpgradePoint ? 1 : 0);
}

void UAbilityGauge::ManaUpdated(const FOnAttributeChangeData& Data)
{
	UpdateCanCast();
}
