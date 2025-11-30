// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "Blueprint/UserWidget.h"
#include "AbilityGauge.generated.h"

/**
 *   虚幻神人设计之不带 IUserObjectListEntry 这个接口是不给你配 UListView表的
 */
UCLASS()
class GREY_API UAbilityGauge : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()
public:
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
private:
	UPROPERTY(meta=(BindWidget))
	class UImage* Icon;

	UPROPERTY(meta=(BindWidget))
	class UTextBlock* CooldownCounterText;

	UPROPERTY(meta=(BindWidget))
	class UTextBlock* CooldownDurationText;

	UPROPERTY(meta=(BindWidget))
	class UTextBlock* CostText;
};
