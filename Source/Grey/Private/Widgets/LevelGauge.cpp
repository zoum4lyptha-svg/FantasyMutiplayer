// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/LevelGauge.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "GAS/GHeroAttributeSet.h"

void ULevelGauge::NativeConstruct()
{
	Super::NativeConstruct();
	//只显示整数位
	NumberFormattingOptions.SetMaximumFractionalDigits(0);

	APawn* OwnerPawn = GetOwningPlayerPawn();
	if (!OwnerPawn)
		return;

	UAbilitySystemComponent* OwnerAbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OwnerPawn);
	if (!OwnerAbilitySystemComponent)
		return;

	OwnerASC = OwnerAbilitySystemComponent;
	
	// 这里注意一下，构造的时候要手动调一次，后续update依赖于委托
	// 客户端AcknowledgePossession 执行构造UI的前提是服务器函数onProgress已经完成，所以这里请求GAS的Attribute是一定可以拿到正确的非0数据的
	// 优雅的设计不需要RPC!
	UpdateGauge(FOnAttributeChangeData());
	OwnerAbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UGHeroAttributeSet::GetExperienceAttribute()).AddUObject(this, &ULevelGauge::UpdateGauge);
	OwnerAbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UGHeroAttributeSet::GetNextLevelExperienceAttribute()).AddUObject(this, &ULevelGauge::UpdateGauge);
	OwnerAbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UGHeroAttributeSet::GetPrevLevelExperienceAttribute()).AddUObject(this, &ULevelGauge::UpdateGauge);
	OwnerAbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UGHeroAttributeSet::GetLevelAttribute()).AddUObject(this, &ULevelGauge::UpdateGauge);
}

void ULevelGauge::UpdateGauge(const FOnAttributeChangeData& Data)
{
	bool bFound;
	float CurrentExperience = OwnerASC->GetGameplayAttributeValue(UGHeroAttributeSet::GetExperienceAttribute(), bFound);
	if (!bFound)
		return;
	float NextLevelExperience = OwnerASC->GetGameplayAttributeValue(UGHeroAttributeSet::GetNextLevelExperienceAttribute(), bFound);
	if (!bFound)
		return;
	float PrevLevelExperience = OwnerASC->GetGameplayAttributeValue(UGHeroAttributeSet::GetPrevLevelExperienceAttribute(), bFound);
	if (!bFound)
		return;
	float CurrentLevel = OwnerASC->GetGameplayAttributeValue(UGHeroAttributeSet::GetLevelAttribute(), bFound);
	if (!bFound)
		return;

	LevelText->SetText(FText::AsNumber(CurrentLevel, &NumberFormattingOptions));

	float Progress = CurrentExperience - PrevLevelExperience;
	float LevelExpAmt = NextLevelExperience - PrevLevelExperience;

	float Percent = Progress / LevelExpAmt;

	if (NextLevelExperience == 0)
	{
		Percent = 1;
	}

	if (LevelProgressImage)
	{
		LevelProgressImage->GetDynamicMaterial()->SetScalarParameterValue(PercentMaterialParamName, Percent);
	}
}