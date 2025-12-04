
#include "GAbilitySystemComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "GAbilitySystemStatics.h"
#include "GameplayEffectExtension.h"
#include "GHeroAttributeSet.h"
#include "PA_AbilitySystemGenerics.h"
#include "GAS/GAttributeSet.h"

void UGAbilitySystemComponent::ApplyInitialEffects()
{
	// 多人下只允许在服务器上 使用GE
	if (!GetOwner() || !GetOwner()->HasAuthority())
		return;
	
	if (!AbilitySystemGenerics)
		return;

	for (const TSubclassOf<UGameplayEffect>& EffectClass : AbilitySystemGenerics->GetInitialEffects())
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
	
	if (!AbilitySystemGenerics)
		return;

	for (const TSubclassOf<UGameplayAbility>& PassiveAbility : AbilitySystemGenerics->GetPassiveAbilities())
	{
		GiveAbility(FGameplayAbilitySpec(PassiveAbility, 1, -1, nullptr));
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
	if (!AbilitySystemGenerics || ! AbilitySystemGenerics->GetBaseStatDataTable() || !GetOwner())
	{
		return;
	}

	const UDataTable* BaseStatDataTable = AbilitySystemGenerics->GetBaseStatDataTable();
	
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
	// Maxlevel, exp 不要 hardCode,从配表中读取
	const FRealCurve* ExperienceCurve = AbilitySystemGenerics->GetExperienceCurve();
	if (ExperienceCurve)
	{
		int MaxLevel = ExperienceCurve->GetNumKeys();
		SetNumericAttributeBase(UGHeroAttributeSet::GetMaxLevelAttribute(), MaxLevel);

		float MaxExp = ExperienceCurve->GetKeyValue(ExperienceCurve->GetLastKeyHandle());
		SetNumericAttributeBase(UGHeroAttributeSet::GetMaxLevelExperienceAttribute(), MaxExp);

		UE_LOG(LogTemp, Warning, TEXT("Max Level is: %d, max experience is: %f"), MaxLevel, MaxExp);
	
		// 初始化时要调一下UpdateExp,否则有一些 attribute没有内容
		ExperienceUpdated(FOnAttributeChangeData());
	}
}


void UGAbilitySystemComponent::ApplyFullStatEffect()
{
	// 服务器应用重生GE
	if (!AbilitySystemGenerics)
		return;
	AuthApplyGameplayEffect(AbilitySystemGenerics->GetFullStatEffect());
}

bool UGAbilitySystemComponent::IsAtMaxLevel() const
{
	bool bFound;
	float CurrentLevel = GetGameplayAttributeValue(UGHeroAttributeSet::GetLevelAttribute(), bFound);
	float MaxLevel = GetGameplayAttributeValue(UGHeroAttributeSet::GetMaxLevelAttribute(), bFound);
	return CurrentLevel >= MaxLevel;
}

void UGAbilitySystemComponent::Server_UpgradeAbilityWithID_Implementation(EGAbilityInputID InputID)
{
	// 升级条件 1，要有升级点 2.当前技能没有到最大等级
	bool bFound = false;
	float UpgradePoint = GetGameplayAttributeValue(UGHeroAttributeSet::GetUpgradePointAttribute(), bFound);
	if (!bFound || UpgradePoint <= 0)
		return;

	FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromInputID((int32)InputID);
	if (!AbilitySpec || UGAbilitySystemStatics::IsAbilityAtMaxLevel(*AbilitySpec))
		return;

	SetNumericAttributeBase(UGHeroAttributeSet::GetUpgradePointAttribute(), UpgradePoint - 1);
	AbilitySpec->Level += 1;
	//注意 这里修改的是 某个ability实例中的level(技能等级)，不是attribute的level(角色等级)
	// AbilitySpec GA实例是很大的，不同于属性的时刻网络同步策略，AbilitySpec是按内存管理的方式对待，只在dirty时同步
	// 所以这里需要手动把服务器标记 level为  dirty, 客户端会在下一帧收到变化并触发监听变化的委托
	MarkAbilitySpecDirty(*AbilitySpec);
}

bool UGAbilitySystemComponent::Server_UpgradeAbilityWithID_Validate(EGAbilityInputID InputID)
{
	return true;
}

void UGAbilitySystemComponent::Client_AbilitySpecLevelUpdated_Implementation(FGameplayAbilitySpecHandle Handle,
	int NewLevel)
{
	FGameplayAbilitySpec* Spec = FindAbilitySpecFromHandle(Handle);
	if (Spec)
	{
		Spec->Level = NewLevel;
		AbilitySpecDirtiedCallbacks.Broadcast(*Spec);
	}
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


			if(AbilitySystemGenerics && AbilitySystemGenerics->GetDeathEffect())
				AuthApplyGameplayEffect(AbilitySystemGenerics->GetDeathEffect());
			
			// 应用死亡GE时，向受害者发送event 触发 Trigger 类型的GAP_death 结算赏金
			FGameplayEventData DeadAbilityEventData;
			if(ChangeData.GEModData)
				DeadAbilityEventData.ContextHandle = ChangeData.GEModData->EffectSpec.GetContext();

			UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetOwner(), UGAbilitySystemStatics::GetDeadStatTag(), DeadAbilityEventData);
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

void UGAbilitySystemComponent::ExperienceUpdated(const FOnAttributeChangeData& ChangeData)
{
	
	if (!GetOwner() || !GetOwner()->HasAuthority())
		return;

	if (IsAtMaxLevel())
		return;

	if (!AbilitySystemGenerics)
		return;

	float CurrentExp = ChangeData.NewValue;

	const FRealCurve* ExperienceCurve = AbilitySystemGenerics->GetExperienceCurve();
	if (!ExperienceCurve)
	{
		UE_LOG(LogTemp, Warning, TEXT("Can't find Experience Data!!"));
		return;
	}

	float PrevLevelExp = 0;
	float NextLevelExp = 0;
	float NewLevel = 1;
	
	for (auto Iter = ExperienceCurve->GetKeyHandleIterator(); Iter; ++Iter)
	{
		// 到达当前第key等级需要的经验
		float ExperienceToReachLevel = ExperienceCurve->GetKeyValue(*Iter);
		if (CurrentExp < ExperienceToReachLevel)
		{
			//目标没有到key级，找到了到达下一级经验的NextLevelExp
			NextLevelExp = ExperienceToReachLevel;
			break;
		}
		// 到达的上一级经验		
		PrevLevelExp = ExperienceToReachLevel;
		NewLevel = Iter.GetIndex() + 1;
	}

	float CurrentLevel = GetNumericAttributeBase(UGHeroAttributeSet::GetLevelAttribute());
	float CurrentUpgradePoint = GetNumericAttribute(UGHeroAttributeSet::GetUpgradePointAttribute());
	
	float LevelUpgraded = NewLevel - CurrentLevel;
	float NewUpgradePoint = CurrentUpgradePoint + LevelUpgraded;

	SetNumericAttributeBase(UGHeroAttributeSet::GetLevelAttribute(), NewLevel);
	SetNumericAttributeBase(UGHeroAttributeSet::GetPrevLevelExperienceAttribute(), PrevLevelExp);
	SetNumericAttributeBase(UGHeroAttributeSet::GetNextLevelExperienceAttribute(), NextLevelExp);
	SetNumericAttributeBase(UGHeroAttributeSet::GetUpgradePointAttribute(), NewUpgradePoint);
}


UGAbilitySystemComponent::UGAbilitySystemComponent()
{

	PrimaryComponentTick.bCanEverTick = true;

	GetGameplayAttributeValueChangeDelegate(UGAttributeSet::GetHealthAttribute()).AddUObject(this, &UGAbilitySystemComponent::HealthUpdated);
	
	GetGameplayAttributeValueChangeDelegate(UGAttributeSet::GetManaAttribute()).AddUObject(this, &UGAbilitySystemComponent::ManaUpdated);
	
	GetGameplayAttributeValueChangeDelegate(UGHeroAttributeSet::GetExperienceAttribute()).AddUObject(this, &UGAbilitySystemComponent::ExperienceUpdated);
	
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

