// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "Abilities/GameplayAbility.h"
#include "GGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class GREY_API UGGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
public:
	UGGameplayAbility();
	//主要为GA子类提供一些方法
protected:
	class UAnimInstance* GetOwnerAnimInstance() const;
	// 根据 (扫描的location数据，碰撞计算范围)，计算碰撞结果
	TArray<FHitResult> GetHitResultFromSweepLocationTargetData(const FGameplayAbilityTargetDataHandle& TargetDataHandle, float SphereSweepRadius = 30.f, ETeamAttitude::Type TargetTeam = ETeamAttitude::Hostile, bool bDrawDebug = false, bool bIgnoreSelf = true) const;

	UFUNCTION()
	FORCEINLINE bool ShouldDrawDebug() const { return bShouldDrawDebug; }

	void PushSelf(const FVector& PushVel);
	void PushTarget(AActor* Target, const FVector& PushVel);

	ACharacter* GetOwningAvatarCharacter();

	void ApplyGameplayEffectToHitResultActor(const FHitResult& HitResult, TSubclassOf<UGameplayEffect> GameplayEffect, int Level = 1);
private:
	UPROPERTY(EditDefaultsOnly, Category = "Debug")
	bool bShouldDrawDebug = false;
	

	UPROPERTY()
	class ACharacter* AvatarCharacter;
};
