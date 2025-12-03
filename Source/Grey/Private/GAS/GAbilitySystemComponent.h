
#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "GAS/GGameplayAbilityTypes.h"
#include "GameplayEffectTypes.h"
#include "GAbilitySystemComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class GREY_API UGAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	void InitializeBaseHeroAttributes();
	void ServerSideInit();

	void ApplyFullStatEffect();
	
	//拿的是角色的独有GA,不拿BasicAbilities
	const TMap<EGAbilityInputID, TSubclassOf<UGameplayAbility>>& GetAbilities() const;
private:
	
	void ApplyInitialEffects();
	void GiveInitialAbilities();
	
	void AuthApplyGameplayEffect(TSubclassOf<UGameplayEffect> GameplayEffect, int Level = 1);


	// 这里放一些初始化ASC时就需要应用的GE
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effects")
	TArray<TSubclassOf<UGameplayEffect>> InitialEffects;

	void HealthUpdated(const FOnAttributeChangeData& ChangeData);
	
	void ManaUpdated(const FOnAttributeChangeData& ChangeData);
	
	// 死亡 GE 单独拿出来配
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effects")
	TSubclassOf<UGameplayEffect> DeathEffect;

	//重生 GE
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effects")
	TSubclassOf<UGameplayEffect> FullStatEffect;
	// 配置需要注册的GA 
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Ability")
	TMap<EGAbilityInputID, TSubclassOf<UGameplayAbility>> Abilities;

	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Ability")
	TMap<EGAbilityInputID, TSubclassOf<UGameplayAbility>> BasicAbilities;

	
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Ability")
	TArray<TSubclassOf<UGameplayAbility>> PassiveAbilities;
	
	
	UPROPERTY(EditDefaultsOnly, Category = "Base Stats")
	UDataTable* BaseStatDataTable;
public:
	
	UGAbilitySystemComponent();

protected:
	
	virtual void BeginPlay() override;

public:
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
};
