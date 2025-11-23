
#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"

#include "GAbilitySystemComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class GREY_API UGAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	void ApplyInitialEffects();

	void GiveInitialAbilities();

private:
	// 这里放一些初始化ASC时就需要应用的GE
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Effects")
	TArray<TSubclassOf<UGameplayEffect>> InitialEffects;

	// 配置需要注册的GA
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Abilitys")
	TArray<TSubclassOf<UGameplayAbility>> Abilities;

	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Abilitys")
	TArray<TSubclassOf<UGameplayAbility>> BasicAbilities;

public:
	
	UGAbilitySystemComponent();

protected:
	
	virtual void BeginPlay() override;

public:
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
};
