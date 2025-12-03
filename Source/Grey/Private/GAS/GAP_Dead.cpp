// Fill out your copyright notice in the Description page of Project Settings.


#include "GAP_Dead.h"

#include "GAbilitySystemStatics.h"
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
		if (Killer)
		{
			UE_LOG(LogTemp, Warning, TEXT("I am Dead, the killer is: %s"), *Killer->GetName());
		}
		
		TArray<AActor*> RewardTargets = GetRewardTargets();
		for (const AActor* RewardTarget : RewardTargets)
		{
			UE_LOG(LogTemp, Warning, TEXT("Find Reward Target: %s"), *RewardTarget->GetName());
		}
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
