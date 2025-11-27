// Fill out your copyright notice in the Description page of Project Settings.


#include "Minion.h"

#include "AbilitySystemComponent.h"
#include "GAS/GAbilitySystemStatics.h"


void AMinion::SetGenericTeamId(const FGenericTeamId& NewTeamId)
{
	Super::SetGenericTeamId(NewTeamId);
	PickSkinBasedOnTeamID();
}

bool AMinion::IsActive() const
{
	return !GetAbilitySystemComponent()->HasMatchingGameplayTag(UGAbilitySystemStatics::GetDeadStatTag());
}

void AMinion::Activate()
{
	GetAbilitySystemComponent()->RemoveActiveEffectsWithGrantedTags(FGameplayTagContainer(UGAbilitySystemStatics::GetDeadStatTag()));
}

void AMinion::PickSkinBasedOnTeamID()
{
	USkeletalMesh** Skin = SkinMap.Find(GetGenericTeamId());
	if (Skin)
	{
		GetMesh()->SetSkeletalMesh(*Skin);
	}
}

void AMinion::OnRep_TeamID()
{
	PickSkinBasedOnTeamID();
}






