
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
	
	bool IsAtMaxLevel() const;
	
	
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_UpgradeAbilityWithID(EGAbilityInputID InputID);

	//这个客户端升级的RPC目前暂时没用，写出来应对未来可能的客户端触发升级的行为。正常来说升级是服务器发生的行为
	UFUNCTION(Client, Reliable)
	void Client_AbilitySpecLevelUpdated(FGameplayAbilitySpecHandle Handle, int NewLevel);
	
	//拿的是角色的独有GA,不拿BasicAbilities
	const TMap<EGAbilityInputID, TSubclassOf<UGameplayAbility>>& GetAbilities() const;
private:
	
	void ApplyInitialEffects();
	void GiveInitialAbilities();
	
	void AuthApplyGameplayEffect(TSubclassOf<UGameplayEffect> GameplayEffect, int Level = 1);

	
	void HealthUpdated(const FOnAttributeChangeData& ChangeData);
	
	void ManaUpdated(const FOnAttributeChangeData& ChangeData);
	
	
	void ExperienceUpdated(const FOnAttributeChangeData& ChangeData);
	
	// 配置需要注册的GA 
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Ability")
	TMap<EGAbilityInputID, TSubclassOf<UGameplayAbility>> Abilities;

	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Ability")
	TMap<EGAbilityInputID, TSubclassOf<UGameplayAbility>> BasicAbilities;

	// 原来初始带的 GE GA data table 现在全部改 读 PrimaryData的表了
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Ability")
	class UPA_AbilitySystemGenerics* AbilitySystemGenerics;
public:
	
	UGAbilitySystemComponent();

protected:
	
	virtual void BeginPlay() override;

public:
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
};
