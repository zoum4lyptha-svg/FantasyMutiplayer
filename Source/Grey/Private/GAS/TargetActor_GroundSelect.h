// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetActor.h"
#include "TargetActor_GroundSelect.generated.h"

UCLASS()
class GREY_API ATargetActor_GroundSelect : public AGameplayAbilityTargetActor
{
	GENERATED_BODY()

public:
	ATargetActor_GroundSelect();

	// 重写 确认和广播委托data的函数，主要是做范围的碰撞查询，过滤敌我
	virtual void ConfirmTargetingAndContinue() override;
	void SetTargetAreaRadius(float NewRadius);
	void SetTargetOptions(bool bTargetFriendly, bool bTargetEnenmy = true);

	
protected:
	
	// 敌我开关
	bool bShouldTargetEnemy = true;
	bool bShouldTargetFriendly = false;
	
	
	virtual void Tick(float DeltaTime) override;
	FVector GetTargetPoint() const;
	
	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float TargetTraceRange = 2000.f;
	
	
	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float TargetAreaRadius = 300.f;
};
