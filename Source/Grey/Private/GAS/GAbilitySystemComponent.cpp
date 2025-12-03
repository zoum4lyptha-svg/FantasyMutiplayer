
#include "GAbilitySystemComponent.h"

#include "GAbilitySystemStatics.h"
#include "GHeroAttributeSet.h"
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


void UGAbilitySystemComponent::ServerSideInit()
{
	// 服务器侧读data table 设置 attribute
	InitializeBaseHeroAttributes();
	ApplyInitialEffects();
	GiveInitialAbilities();
}

void UGAbilitySystemComponent::InitializeBaseHeroAttributes()
{
	if (!BaseStatDataTable || !GetOwner())
	{
		return;
	}

	const FHeroBaseStats* BaseStats = nullptr;

	for (const TPair<FName, uint8*>& DataPair : BaseStatDataTable->GetRowMap())
	{
		BaseStats = BaseStatDataTable->FindRow<FHeroBaseStats>(DataPair.Key, "");
		if (BaseStats && BaseStats->Class == GetOwner()->GetClass())
		{
			break;
		}
	}

	if (BaseStats)
	{
		// 根据 table 中的 数据 设置 attribute
		SetNumericAttributeBase(UGAttributeSet::GetMaxHealthAttribute(), BaseStats->BaseMaxHealth);
		SetNumericAttributeBase(UGAttributeSet::GetMaxManaAttribute(), BaseStats->BaseMaxMana);
		SetNumericAttributeBase(UGAttributeSet::GetAttackDamageAttribute(), BaseStats->BaseAttackDamage);
		SetNumericAttributeBase(UGAttributeSet::GetArmorAttribute(), BaseStats->BaseArmor);
		SetNumericAttributeBase(UGAttributeSet::GetMoveSpeedAttribute(), BaseStats->BaseMoveSpeed);

		SetNumericAttributeBase(UGHeroAttributeSet::GetStrengthAttribute(), BaseStats->Strength);
		SetNumericAttributeBase(UGHeroAttributeSet::GetStrengthGrowthRateAttribute(), BaseStats->StrengthGrowthRate);
		SetNumericAttributeBase(UGHeroAttributeSet::GetIntelligenceAttribute(), BaseStats->Intelligence);
		SetNumericAttributeBase(UGHeroAttributeSet::GetIntelligenceGrowthRateAttribute(), BaseStats->IntelligenceGrowthRate);
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
	// update 限定服务器逻辑
	if (!GetOwner() || !GetOwner()->HasAuthority()) return;

	bool bFound = false;
	float MaxHealth = GetGameplayAttributeValue(UGAttributeSet::GetMaxHealthAttribute(), bFound);
	if (bFound && ChangeData.NewValue >= MaxHealth)
	{
		if (!HasMatchingGameplayTag(UGAbilitySystemStatics::GetHealthFullStatTag()))
		{
			//This is done local only.(客户端本地不需要同步这类full TAG)
			AddLooseGameplayTag(UGAbilitySystemStatics::GetHealthFullStatTag());
		}
	}
	else
	{
		RemoveLooseGameplayTag(UGAbilitySystemStatics::GetHealthFullStatTag());
	}

	if (ChangeData.NewValue <= 0)
	{
		if (!HasMatchingGameplayTag(UGAbilitySystemStatics::GetHealthEmptyStatTag()))
		{
			AddLooseGameplayTag(UGAbilitySystemStatics::GetHealthEmptyStatTag());

			if(DeathEffect)
				AuthApplyGameplayEffect(DeathEffect);
		}
	}
	else
	{
		RemoveLooseGameplayTag(UGAbilitySystemStatics::GetHealthEmptyStatTag());
	}
}

void UGAbilitySystemComponent::ManaUpdated(const FOnAttributeChangeData& ChangeData)
{
	if (!GetOwner() || !GetOwner()->HasAuthority()) return;

	bool bFound = false;
	float MaxMana = GetGameplayAttributeValue(UGAttributeSet::GetMaxManaAttribute(), bFound);
	if (bFound && ChangeData.NewValue >= MaxMana)
	{
		if (!HasMatchingGameplayTag(UGAbilitySystemStatics::GetManaFullStatTag()))
		{
			//添加/删除 全满 tag
			AddLooseGameplayTag(UGAbilitySystemStatics::GetManaFullStatTag());
		}
	}
	else
	{
		RemoveLooseGameplayTag(UGAbilitySystemStatics::GetManaFullStatTag());
	}

	if (ChangeData.NewValue <= 0)
	{
		if (!HasMatchingGameplayTag(UGAbilitySystemStatics::GetManaEmptyStatTag()))
		{
			// 添加/删除 空血tag
			AddLooseGameplayTag(UGAbilitySystemStatics::GetManaEmptyStatTag());
		}
	}
	else
	{
		RemoveLooseGameplayTag(UGAbilitySystemStatics::GetManaEmptyStatTag());
	}
}


UGAbilitySystemComponent::UGAbilitySystemComponent()
{

	PrimaryComponentTick.bCanEverTick = true;

	GetGameplayAttributeValueChangeDelegate(UGAttributeSet::GetHealthAttribute()).AddUObject(this, &UGAbilitySystemComponent::HealthUpdated);
	
	GetGameplayAttributeValueChangeDelegate(UGAttributeSet::GetManaAttribute()).AddUObject(this, &UGAbilitySystemComponent::ManaUpdated);
	
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

