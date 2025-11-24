// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AN_SendGameplayEvent.generated.h"

/**
 * 
 */
UCLASS()
class GREY_API UAN_SendGameplayEvent : public UAnimNotify
{
	GENERATED_BODY()

	// 工具父类 定义向 animtion 的 owner 对象的ASC 发送 gameplay event
public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
private:
	UPROPERTY(EditAnywhere, Category = "Gameplay Ability")
	FGameplayTag EventTag;
	virtual FString GetNotifyName_Implementation() const; 
};
