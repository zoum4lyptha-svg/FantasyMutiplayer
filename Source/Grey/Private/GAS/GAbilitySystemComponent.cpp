
#include "GAbilitySystemComponent.h"
#include "GAS/GAttributeSet.h"

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
	for (const TPair<EGAbilityInputID,TSubclassOf<UGameplayAbility>>& AbilityPair : Abilities)
	{
		GiveAbility(FGameplayAbilitySpec(AbilityPair.Value, 0, (int32)AbilityPair.Key, nullptr));
	}

	// 注册基础就有的GA
	for (const TPair<EGAbilityInputID,TSubclassOf<UGameplayAbility>>& AbilityPair : BasicAbilities)
	{
		GiveAbility(FGameplayAbilitySpec(AbilityPair.Value, 1, (int32)AbilityPair.Key, nullptr));
	}
}

void UGAbilitySystemComponent::ApplyFullStatEffect()
{
	// 服务器应用重生GE
	AuthApplyGameplayEffect(FullStatEffect);
}

const TMap<EGAbilityInputID, TSubclassOf<UGameplayAbility>>& UGAbilitySystemComponent::GetAbilities() const
{
	return Abilities;
}

void UGAbilitySystemComponent::AuthApplyGameplayEffect(TSubclassOf<UGameplayEffect> GameplayEffect, int Level)
{
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		FGameplayEffectSpecHandle EffectSpecHandle = MakeOutgoingSpec(GameplayEffect, Level, MakeEffectContext());
		ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());
	}
}

void UGAbilitySystemComponent::HealthUpdated(const FOnAttributeChangeData& ChangeData)
{
	if (!GetOwner()) return;

	if (ChangeData.NewValue <= 0 && GetOwner()->HasAuthority() && DeathEffect)
	{
		//在服务器应用死亡GE
		AuthApplyGameplayEffect(DeathEffect);
	}
}


UGAbilitySystemComponent::UGAbilitySystemComponent()
{

	PrimaryComponentTick.bCanEverTick = true;

	GetGameplayAttributeValueChangeDelegate(UGAttributeSet::GetHealthAttribute()).AddUObject(this, &UGAbilitySystemComponent::HealthUpdated);
	
	//使用 target actor 时 要给引擎内部的确认 / 取消 的输入通道 绑定到 枚举 （枚举会被映射到增强输入）
	GenericConfirmInputID = (int32)EGAbilityInputID::Confirm;
	GenericCancelInputID = (int32)EGAbilityInputID::Cancel;
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

