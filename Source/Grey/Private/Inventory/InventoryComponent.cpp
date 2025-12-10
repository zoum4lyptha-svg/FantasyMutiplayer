// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "InventoryItem.h"
#include "GAS/GHeroAttributeSet.h"
#include "Inventory/PA_ShopItem.h"


UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	
}

void UInventoryComponent::TryPurchase(const UPA_ShopItem* ItemToPurchase)
{
	if (!OwnerAbilitySystemComponent)
		return;

	// 注意 TryPurchase 有一点多端细节
	// 设计上服务器不画 UI，模拟客户端也不画UI,所以点击委托只可能在主控客户端触发。所以TryPurchase执行方只有 主控客户端 
	Server_Purchase(ItemToPurchase);
}



// Called when the game starts
void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerAbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner());
	
}

// server only
void UInventoryComponent::GrantItem(const UPA_ShopItem* NewItem)
{
	if (!GetOwner()->HasAuthority())
		return;

	UInventoryItem* InventoryItem = NewObject<UInventoryItem>();
	FInventoryItemHandle NewHandle = FInventoryItemHandle::CreateHandle();
	InventoryItem->InitItem(NewHandle, NewItem);
	InventoryMap.Add(NewHandle, InventoryItem);
	
	OnItemAdded.Broadcast(InventoryItem);
	UE_LOG(LogTemp, Warning, TEXT("Server Adding Shop Item: %s, with Id: %d"), *(InventoryItem->GetShopItem()->GetItemName().ToString()), NewHandle.GetHandleId());
	// 客户端 RPC 建一个一样的 handle map实现双端只用handle同步
	Client_ItemAdded(NewHandle, NewItem);
}


//client only
void UInventoryComponent::Client_ItemAdded_Implementation(FInventoryItemHandle AssignedHandle, const UPA_ShopItem* Item)
{
	if (GetOwner()->HasAuthority())
		return;

	UInventoryItem* InventoryItem = NewObject<UInventoryItem>();
	InventoryItem->InitItem(AssignedHandle, Item);
	InventoryMap.Add(AssignedHandle, InventoryItem);
	OnItemAdded.Broadcast(InventoryItem);
	UE_LOG(LogTemp, Warning, TEXT("Client Adding Shop Item: %s, with Id: %d"), *(InventoryItem->GetShopItem()->GetItemName().ToString()), AssignedHandle.GetHandleId());
}

void UInventoryComponent::Server_Purchase_Implementation(const UPA_ShopItem* ItemToPurchase)
{
	// 目前只有 校验 gold
	if (!ItemToPurchase)
		return;
	
	if (GetGold() < ItemToPurchase->GetPrice())
		return;

	OwnerAbilitySystemComponent->ApplyModToAttribute(UGHeroAttributeSet::GetGoldAttribute(), EGameplayModOp::Additive, -ItemToPurchase->GetPrice());
	
	GrantItem(ItemToPurchase);
}

bool UInventoryComponent::Server_Purchase_Validate(const UPA_ShopItem* ItemToPurchase)
{
	return true;
}



float UInventoryComponent::GetGold() const
{
	bool bFound = false;
	if (OwnerAbilitySystemComponent)
	{
		float Gold = OwnerAbilitySystemComponent->GetGameplayAttributeValue(UGHeroAttributeSet::GetGoldAttribute(), bFound);
		if (bFound)
		{
			return Gold;
		}
	}

	return 0.f;
}
