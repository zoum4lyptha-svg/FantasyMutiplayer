// Fill out your copyright notice in the Description page of Project Settings.


#include "GAP_Dead.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GAbilitySystemStatics.h"
#include "GHeroAttributeSet.h"
#include "Engine/OverlapResult.h"

UGAP_Dead::UGAP_Dead()
{
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;

	FAbilityTriggerData TriggerData;
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	TriggerData.TriggerTag = UGAbilitySystemStatics::GetDeadStatTag();

	AbilityTriggers.Add(TriggerData);

	// 记得把晕眩的block关了
	ActivationBlockedTags.RemoveTag(UGAbilitySystemStatics::GetStunStatTag());
}

void UGAP_Dead::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	
	if (K2_HasAuthority())
	{
		AActor* Killer = TriggerEventData->ContextHandle.GetEffectCauser();
		// DEBUG: 注意killer有可能是小兵... 不需要作为赏金瓜分的一份子
		if (!Killer || !UGAbilitySystemStatics::IsHero(Killer))
		{
			Killer = nullptr;
		}
		
		TArray<AActor*> RewardTargets = GetRewardTargets();
		if (RewardTargets.Num() == 0 && !Killer)
		{
			K2_EndAbility();
			return;
		}
		
		// 兜底逻辑 有时killer因为距离过远，在扫描范围外没有被添加
		if (Killer && !RewardTargets.Contains(Killer))
		{
			RewardTargets.Add(Killer);
		}
		
		bool bFound = false;
		float SelfExperience = GetAbilitySystemComponentFromActorInfo_Ensured()->GetGameplayAttributeValue(UGHeroAttributeSet::GetExperienceAttribute(), bFound);

		// ExperienceRewardPerExperience 与被击杀者相关的额外经验
		float TotalExperienceReward = BaseExperienceReward + ExperienceRewardPerExperience * SelfExperience;
		float TotalGoldReward = BaseGoldReward + GoldRewardPerExperience * SelfExperience;

		if (Killer)
		{
			// 先由击杀者分走KillerRewardPortion比例的奖励
			float KillerExperienceReward = TotalExperienceReward * KillerRewardPortion;
			float KillerGoldReward = TotalGoldReward * KillerRewardPortion;

			// 注意 这里根据block的规则，如果击杀者死亡，是不会应用这个奖励的
			FGameplayEffectSpecHandle EffectSpec = MakeOutgoingGameplayEffectSpec(RewardEffect);
			
			// SetByCaller 把数值（exp，gold奖励）跟随 TAG 发给 killer，蓝图GE读取直接改
			EffectSpec.Data->SetSetByCallerMagnitude(UGAbilitySystemStatics::GetExperienceAttributeTag(), KillerExperienceReward);
			EffectSpec.Data->SetSetByCallerMagnitude(UGAbilitySystemStatics::GetGoldAttributeTag(), KillerGoldReward);

			K2_ApplyGameplayEffectSpecToTarget(EffectSpec, UAbilitySystemBlueprintLibrary::AbilityTargetDataFromActor(Killer));

			TotalExperienceReward -= KillerExperienceReward;
			TotalGoldReward -= KillerGoldReward;
		}

		// 剩余奖励由剩下的nums瓜分
		float ExperiencePerTarget = TotalExperienceReward / RewardTargets.Num();
		float GoldPerTarget = TotalGoldReward / RewardTargets.Num();

		FGameplayEffectSpecHandle EffectSpec = MakeOutgoingGameplayEffectSpec(RewardEffect);
		
		
		EffectSpec.Data->SetSetByCallerMagnitude(UGAbilitySystemStatics::GetExperienceAttributeTag(), ExperiencePerTarget);
		EffectSpec.Data->SetSetByCallerMagnitude(UGAbilitySystemStatics::GetGoldAttributeTag(), GoldPerTarget);

		K2_ApplyGameplayEffectSpecToTarget(EffectSpec, UAbilitySystemBlueprintLibrary::AbilityTargetDataFromActorArray(RewardTargets, true));
		K2_EndAbility();
	}
}

// 寻找 RewardRange 半径内的友方英雄
TArray<AActor*> UGAP_Dead::GetRewardTargets() const
{
	TSet<AActor*> OutActors;

	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!AvatarActor || !GetWorld())
	{
		return OutActors.Array();
	}

	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECC_Pawn);
	FCollisionShape CollisionShape;
	CollisionShape.SetSphere(RewardRange);

	// 球形，pawn类型的 多结果碰撞检查
	TArray<FOverlapResult> OverlapResults;
	if (GetWorld()->OverlapMultiByObjectType(OverlapResults, AvatarActor->GetActorLocation(), FQuat::Identity, ObjectQueryParams, CollisionShape))
	{
		for (const FOverlapResult& OverlapResult : OverlapResults)
		{
			const IGenericTeamAgentInterface* OtherTeamInterface = Cast<IGenericTeamAgentInterface>(OverlapResult.GetActor());
			if (!OtherTeamInterface || OtherTeamInterface->GetTeamAttitudeTowards(*AvatarActor) != ETeamAttitude::Hostile)
			{
				continue;
			}
			// 与攻击者不是敌人 && 是英雄单位
			// todo:这里有个小坑，后续可能会出英雄AI托管，所以有没有AI control并不能成为判断英雄依据
			if (!UGAbilitySystemStatics::IsHero(OverlapResult.GetActor()))
			{
				continue;
			}

			OutActors.Add(OverlapResult.GetActor());
		}
	}

	return OutActors.Array();
}
