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

protected:
	
	virtual void Tick(float DeltaTime) override;
	FVector GetTargetPoint() const;
	
	UPROPERTY(EditDefaultsOnly, Category = "Targeting")
	float TargetTraceRange = 2000.f;
	
};
