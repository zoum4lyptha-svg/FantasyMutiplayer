// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "GHeroAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
 	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
 	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
 	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
 	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)
/**
 * 
 */
UCLASS()
class UGHeroAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
public:
    ATTRIBUTE_ACCESSORS(UGHeroAttributeSet, Intelligence)
    ATTRIBUTE_ACCESSORS(UGHeroAttributeSet, Strength)
    ATTRIBUTE_ACCESSORS(UGHeroAttributeSet, Experience)
    ATTRIBUTE_ACCESSORS(UGHeroAttributeSet, PrevLevelExperience)
    ATTRIBUTE_ACCESSORS(UGHeroAttributeSet, NextLevelExperience)
    ATTRIBUTE_ACCESSORS(UGHeroAttributeSet, Level)
	ATTRIBUTE_ACCESSORS(UGHeroAttributeSet, UpgradePoint)
    ATTRIBUTE_ACCESSORS(UGHeroAttributeSet, MaxLevel)
	ATTRIBUTE_ACCESSORS(UGHeroAttributeSet, MaxLevelExperience)
    ATTRIBUTE_ACCESSORS(UGHeroAttributeSet, Gold)
	ATTRIBUTE_ACCESSORS(UGHeroAttributeSet, StrengthGrowthRate)
	ATTRIBUTE_ACCESSORS(UGHeroAttributeSet, IntelligenceGrowthRate)
	virtual void GetLifetimeReplicatedProps( TArray< class FLifetimeProperty > & OutLifetimeProps ) const override;
private:
	UPROPERTY(ReplicatedUsing = OnRep_Intelligence)
	FGameplayAttributeData Intelligence;

	UPROPERTY(ReplicatedUsing = OnRep_Strength)
	FGameplayAttributeData Strength;
	
	UPROPERTY(ReplicatedUsing = OnRep_Experience)
	FGameplayAttributeData Experience;

	UPROPERTY(ReplicatedUsing = OnRep_PrevLevelExperience)
	FGameplayAttributeData PrevLevelExperience;

	UPROPERTY(ReplicatedUsing = OnRep_NextLevelExperience)
	FGameplayAttributeData NextLevelExperience;
	
	UPROPERTY()
	FGameplayAttributeData StrengthGrowthRate;
	
	UPROPERTY()
	FGameplayAttributeData IntelligenceGrowthRate;


	UPROPERTY(ReplicatedUsing = OnRep_Level)
	FGameplayAttributeData Level;
	
	
	UPROPERTY(ReplicatedUsing = OnRep_UpgradePoint)
	FGameplayAttributeData UpgradePoint;
	
	
	UPROPERTY(ReplicatedUsing = OnRep_MaxLevel)
	FGameplayAttributeData MaxLevel;

	
	UPROPERTY(ReplicatedUsing = OnRep_MaxLevelExperience)
	FGameplayAttributeData MaxLevelExperience;
	
	
	UPROPERTY(ReplicatedUsing = OnRep_Gold)
	FGameplayAttributeData Gold;

	UFUNCTION()
	void OnRep_Intelligence(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_Strength(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_Experience(const FGameplayAttributeData& OldValue);

	
	UFUNCTION()
	void OnRep_UpgradePoint(const FGameplayAttributeData& OldValue);
	
	UFUNCTION()
	void OnRep_MaxLevelExperience(const FGameplayAttributeData& OldValue);
	
	
	UFUNCTION()
	void OnRep_PrevLevelExperience(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_NextLevelExperience(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_Level(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_MaxLevel(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_Gold(const FGameplayAttributeData& OldValue);
};