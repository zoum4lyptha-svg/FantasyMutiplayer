// Fill out your copyright notice in the Description page of Project Settings.


#include "GAP_Launched.h"

UGAP_Launched::UGAP_Launched()
{
	// 升空是被动触发技能 (GAP)最好关了本地预测，因为客户端actor会同步位置，所以不同步GA也不会有问题
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;
	
	FAbilityTriggerData TriggerData;
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	TriggerData.TriggerTag = GetLauchedAbilityActiationTag();

	AbilityTriggers.Add(TriggerData);
}

void UGAP_Launched::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                        const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                        const FGameplayEventData* TriggerEventData)
{
	// 这里check一下，为冷却做铺垫
	if (!K2_CommitAbility())
	{
		K2_EndAbility();
		return;
	}

	if (K2_HasAuthority())
	{
		// todo: 后期考虑加入Motion Warping
		// ImpactNormal升空方向目前暂定为碰撞法线
		PushSelf(TriggerEventData->TargetData.Get(0)->GetHitResult()->ImpactNormal);
		K2_EndAbility();
	}
}

FGameplayTag UGAP_Launched::GetLauchedAbilityActiationTag()
{
	return FGameplayTag::RequestGameplayTag("ability.passive.launch.activate");
}
