// Fill out your copyright notice in the Description page of Project Settings.


#include "GGameplayAbility.h"
#include "Components/SkeletalMeshComponent.h"

class UAnimInstance* UGGameplayAbility::GetOwnerAnimInstance() const
{
	USkeletalMeshComponent* OwnerSkeletalMeshComp = GetOwningComponentFromActorInfo();
	if (OwnerSkeletalMeshComp)
	{
		return OwnerSkeletalMeshComp->GetAnimInstance();
	}
	return nullptr;
}

