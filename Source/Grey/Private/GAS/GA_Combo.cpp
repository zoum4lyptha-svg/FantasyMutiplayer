// Fill out your copyright notice in the Description page of Project Settings.


#include "GA_Combo.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "GAS/GAbilitySystemStatics.h"


UGA_Combo::UGA_Combo()
{
	//构造时 添加TAG,添加 block tag , all in C++
	AbilityTags.AddTag(UGAbilitySystemStatics::GetBasicAttackAbilityTag());
	BlockAbilitiesWithTag.AddTag(UGAbilitySystemStatics::GetBasicAttackAbilityTag());
}

void UGA_Combo::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{

	// 改为蓝图 K2版本 内部 调 C++ 版的CommitAbility
	if (!K2_CommitAbility())
	{
		// 官方推荐激活 GA失败也要手动调结束
		K2_EndAbility();
		return;
	}

	// 注意，1.UAbilityTask_PlayMontageAndWait有多端同时执行的本地预测，回撤等逻辑，内部会同步在所有客户端和服务器的player上播放动画
	// 2. Authority 服务器  PredictionKey 主控客户端（需要走这个task的逻辑 算动画状态，搞本地预测） ，其他 simulation proxty 等服务器同步就行了，不需要预测
	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		UAbilityTask_PlayMontageAndWait* PlayComboMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, ComboMontage);
		PlayComboMontageTask->OnBlendOut.AddDynamic(this, &UGA_Combo::K2_EndAbility);
		PlayComboMontageTask->OnCancelled.AddDynamic(this, &UGA_Combo::K2_EndAbility);
		PlayComboMontageTask->OnCompleted.AddDynamic(this, &UGA_Combo::K2_EndAbility);
		PlayComboMontageTask->OnInterrupted.AddDynamic(this, &UGA_Combo::K2_EndAbility);
		PlayComboMontageTask->ReadyForActivation();
	}
}
