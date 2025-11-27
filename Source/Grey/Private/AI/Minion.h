// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/GCharacter.h"
#include "Minion.generated.h"

UCLASS()
class GREY_API AMinion : public AGCharacter
{
	GENERATED_BODY()

public:
	virtual void SetGenericTeamId(const FGenericTeamId& NewTeamId) override;
private:
	void PickSkinBasedOnTeamID();

	// 目前暂定只有AI才需要换外观。每次teamID变化时 重新加载对应的皮肤
	virtual void OnRep_TeamID() override;

	UPROPERTY(EditDefaultsOnly, Category = "Visual skin")
	TMap<FGenericTeamId, USkeletalMesh*> SkinMap;
};
