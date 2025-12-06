// Fill out your copyright notice in the Description page of Project Settings.


#include "GAbilitySystemStatics.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayEffect.h"
#include "Abilities/GameplayAbility.h"

FGameplayTag UGAbilitySystemStatics::GetBasicAttackAbilityTag()
{
	return FGameplayTag::RequestGameplayTag("ability.basicattack");
}

FGameplayTag UGAbilitySystemStatics::GetDeadStatTag()
{
	return FGameplayTag::RequestGameplayTag("stats.dead");
}

FGameplayTag UGAbilitySystemStatics::GetStunStatTag()
{
	return FGameplayTag::RequestGameplayTag("stats.stun");
}

FGameplayTag UGAbilitySystemStatics::GetBasicAttackInputPressedTag()
{
	return FGameplayTag::RequestGameplayTag("ability.basicattack.pressed");
}

float UGAbilitySystemStatics::GetStaticCooldownDurationForAbility(const UGameplayAbility* Ability)
{
	if (!Ability)
		return 0.f;
	
	const UGameplayEffect* CooldownEffect = Ability->GetCooldownGameplayEffect();
	if (!CooldownEffect)
		return 0.f;

	float CooldownDuration = 0.f;

	CooldownEffect->DurationMagnitude.GetStaticMagnitudeIfPossible(1, CooldownDuration);
	return CooldownDuration;
}

float UGAbilitySystemStatics::GetStaticCostForAbility(const UGameplayAbility* Ability)
{
	if (!Ability)
		return 0.f;

	const UGameplayEffect* CostEffect = Ability->GetCostGameplayEffect();
	if (!CostEffect || CostEffect->Modifiers.Num() == 0)
		return 0.f;

	float Cost = 0.f;
	CostEffect->Modifiers[0].ModifierMagnitude.GetStaticMagnitudeIfPossible(1, Cost);
	return FMath::Abs(Cost);
}

FGameplayTag UGAbilitySystemStatics::GetAimStatTag()
{
	return FGameplayTag::RequestGameplayTag("stats.aim");
}

FGameplayTag UGAbilitySystemStatics::GetCameraShakeGameplayCueTag()
{
	return FGameplayTag::RequestGameplayTag("GameplayCue.cameraShake");
}

FGameplayTag UGAbilitySystemStatics::GetHealthFullStatTag()
{
	return FGameplayTag::RequestGameplayTag("stats.health.full");
}

FGameplayTag UGAbilitySystemStatics::GetHealthEmptyStatTag()
{
	return FGameplayTag::RequestGameplayTag("stats.health.empty");
}

FGameplayTag UGAbilitySystemStatics::GetManaFullStatTag()
{
	return FGameplayTag::RequestGameplayTag("stats.mana.full");
}

FGameplayTag UGAbilitySystemStatics::GetManaEmptyStatTag()
{
	return FGameplayTag::RequestGameplayTag("stats.mana.empty");
}

FGameplayTag UGAbilitySystemStatics::GetExperienceAttributeTag()
{
	return FGameplayTag::RequestGameplayTag("attr.experience");
}

FGameplayTag UGAbilitySystemStatics::GetGoldAttributeTag()
{
	return FGameplayTag::RequestGameplayTag("attr.gold");
}

FGameplayTag UGAbilitySystemStatics::GetHeroRoleTag()
{
	return FGameplayTag::RequestGameplayTag("role.hero");
}

bool UGAbilitySystemStatics::IsHero(const AActor* ActorToCheck)
{
	const IAbilitySystemInterface* ActorISA = Cast<IAbilitySystemInterface>(ActorToCheck);
	if (ActorISA)
	{
		UAbilitySystemComponent* ActorASC = ActorISA->GetAbilitySystemComponent();
		if (ActorASC)
		{
			return ActorASC->HasMatchingGameplayTag(GetHeroRoleTag());
		}
	}
	return false;
}

bool UGAbilitySystemStatics::IsAbilityAtMaxLevel(const FGameplayAbilitySpec& Spec)
{
	return Spec.Level >= 4;
}

bool UGAbilitySystemStatics::CheckAbilityCost(const FGameplayAbilitySpec& AbilitySpec,
	const UAbilitySystemComponent& ASC)
{
	const UGameplayAbility* AbilityCDO = AbilitySpec.Ability;
	if (AbilityCDO)
	{
		return AbilityCDO->CheckCost(AbilitySpec.Handle, ASC.AbilityActorInfo.Get());
	}

	return false;
}

// 类似于之前的GetStaticCostForAbility，是从GA的CDO中拿静态配好的CostGE
// 但是注意这里构建了一个临时的cost GE,AttemptCalculateMagnitude内部会根据你配的Magnitude计算出一个运行时实际的消耗值
// 所以ManaCost是动态的值
float UGAbilitySystemStatics::GetManaCostFor(const UGameplayAbility* AbilityCDO, const UAbilitySystemComponent& ASC,
	int AbilityLevel)
{
	float ManaCost = 0.f;
	if (AbilityCDO)
	{
		UGameplayEffect* CostEffect = AbilityCDO->GetCostGameplayEffect();
		if (CostEffect)
		{
			FGameplayEffectSpecHandle EffectSpec = ASC.MakeOutgoingSpec(CostEffect->GetClass(), AbilityLevel, ASC.MakeEffectContext());
			CostEffect->Modifiers[0].ModifierMagnitude.AttemptCalculateMagnitude(*EffectSpec.Data.Get(), ManaCost);
		}
	}

	return FMath::Abs(ManaCost);
}

// 同上思路，拿到的是模拟的动态值 最大冷却
float UGAbilitySystemStatics::GetCooldownDurationFor(const UGameplayAbility* AbilityCDO,
	const UAbilitySystemComponent& ASC, int AbilityLevel)
{
	
	float CooldownDuration = 0.f;
	if (AbilityCDO)
	{
		UGameplayEffect* CooldownEffect = AbilityCDO->GetCooldownGameplayEffect();
		if (CooldownEffect)
		{
			FGameplayEffectSpecHandle EffectSpec = ASC.MakeOutgoingSpec(CooldownEffect->GetClass(), AbilityLevel, ASC.MakeEffectContext());
			CooldownEffect->DurationMagnitude.AttemptCalculateMagnitude(*EffectSpec.Data.Get(), CooldownDuration);
		}
	}

	return FMath::Abs(CooldownDuration);
}

// 动态的 冷却剩余时间
float UGAbilitySystemStatics::GetCooldownRemainingFor(const UGameplayAbility* AbilityCDO,
	const UAbilitySystemComponent& ASC)
{
	if (!AbilityCDO)
		return 0;

	UGameplayEffect* CooldownEffect = AbilityCDO->GetCooldownGameplayEffect();
	if (!CooldownEffect)
		return 0;

	FGameplayEffectQuery CooldownEffectQuery;
	CooldownEffectQuery.EffectDefinition = CooldownEffect->GetClass();

	float CooldownRemaining = 0.f;
	// 这里稍微有点绕，是根据你目标GE的类型（从GA 的 CDO里拿到的配表的CooldownEffect） 
	// ASC中有容器保存当前实际处于激活的GE(ActiveGameplayEffects),查找你目标类型GE当前的冷却剩余多少
	// 所以是绝对准确的动态的冷却剩余时间
	FJsonSerializableArrayFloat CooldownTimeRemainings = ASC.GetActiveEffectsTimeRemaining(CooldownEffectQuery);

	for (float Remaining : CooldownTimeRemainings)
	{
		// 注意: GE是默认可以叠加的，连续应用多次GE，会刷新计时器，所以默认只有一个Cooldown
		// 这里是兜底逻辑，应对GE不堆叠的设置，找到最大的计时器
		if (Remaining > CooldownRemaining)
		{
			CooldownRemaining = Remaining;
		}
	}

	return CooldownRemaining;
}
