
#include "GAbilitySystemComponent.h"


void UGAbilitySystemComponent::ApplyInitialEffects()
{
	// 多人下只允许在服务器上 使用GE
	if (!GetOwner() || !GetOwner()->HasAuthority())
		return;

	// 应用GE
	for (const TSubclassOf<UGameplayEffect>& EffectClass : InitialEffects)
	{
		FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingSpec(EffectClass, 1, MakeEffectContext());
		ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());
	}
}

void UGAbilitySystemComponent::GiveInitialAbilities()
{

	// 服务器才能注册 GA
	if (!GetOwner() || !GetOwner()->HasAuthority())
		return;

	// 注册游戏开始时还没有学会的GA
	for (const TSubclassOf<UGameplayAbility>& AbilityClass : Abilities)
	{
		GiveAbility(FGameplayAbilitySpec(AbilityClass, 0, -1, nullptr));
	}

	// 注册基础就有的GA
	for (const TSubclassOf<UGameplayAbility>& AbilityClass : BasicAbilities)
	{
		GiveAbility(FGameplayAbilitySpec(AbilityClass, 1, -1, nullptr));
	}
}


UGAbilitySystemComponent::UGAbilitySystemComponent()
{

	PrimaryComponentTick.bCanEverTick = true;

	
}



void UGAbilitySystemComponent::BeginPlay()
{
	Super::BeginPlay();

	
	
}



void UGAbilitySystemComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                             FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	
}

