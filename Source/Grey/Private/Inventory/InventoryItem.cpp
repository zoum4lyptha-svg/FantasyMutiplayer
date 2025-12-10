// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryItem.h"

#include "AbilitySystemComponent.h"
#include "PA_ShopItem.h"

FInventoryItemHandle::FInventoryItemHandle()
: HandleId{GetInvalidId()}
{
}


FInventoryItemHandle::FInventoryItemHandle(uint32 Id)
: HandleId{Id}
{
}

FInventoryItemHandle FInventoryItemHandle::InvalidHandle()
{
	static FInventoryItemHandle InvalidHandle = FInventoryItemHandle();
	return InvalidHandle;
}

FInventoryItemHandle FInventoryItemHandle::CreateHandle()
{
	return FInventoryItemHandle(GenerateNextId());
}

bool FInventoryItemHandle::IsValid() const
{
	return HandleId != GetInvalidId();
}


uint32 FInventoryItemHandle::GenerateNextId()
{
	static uint32 StaticId = 1;
	return StaticId++;
}

uint32 FInventoryItemHandle::GetInvalidId()
{
	return 0;
}

bool operator==(const FInventoryItemHandle& Lhs, const FInventoryItemHandle& Rhs)
{
	return Lhs.GetHandleId() == Rhs.GetHandleId();
}

uint32 GetTypeHash(const FInventoryItemHandle& Key)
{
	return Key.GetHandleId();
}

void UInventoryItem::InitItem(const FInventoryItemHandle& NewHandle, const UPA_ShopItem* NewShopItem)
{
	Handle = NewHandle;
	ShopItem = NewShopItem;
}

void UInventoryItem::ApplyGasModifications(UAbilitySystemComponent* AbilitySystemComponent)
{
	if (!GetShopItem() || !AbilitySystemComponent)
		return;

	if (!AbilitySystemComponent->GetOwner() || !AbilitySystemComponent->GetOwner()->HasAuthority())
		return;

	TSubclassOf<UGameplayEffect> EquipEffect = GetShopItem()->GetEquippedEffect();
	if (EquipEffect)
	{
		AppliedEquipedEffectHandle = AbilitySystemComponent->BP_ApplyGameplayEffectToSelf(EquipEffect, 1, AbilitySystemComponent->MakeEffectContext());
	}

	TSubclassOf<UGameplayAbility> GrantedAbility = GetShopItem()->GetGrantedAbility();
	if (GrantedAbility)
	{
		// 注册GA的时候需要注意 可能目标对象已经有这种GA spec了，需要保证不重复添加
		const FGameplayAbilitySpec* FoundSpec = AbilitySystemComponent->FindAbilitySpecFromClass(GrantedAbility);
		if (FoundSpec)
		{
			GrantedAbiltiySpecHandle = FoundSpec->Handle;
		}
		else
		{
			GrantedAbiltiySpecHandle = AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(GrantedAbility));
		}
	}
}



