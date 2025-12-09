// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
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

void UInventoryComponent::Server_Purchase_Implementation(const UPA_ShopItem* ItemToPurchase)
{
	// 目前只有 校验 gold
	if (!ItemToPurchase)
		return;
	
	if (GetGold() < ItemToPurchase->GetPrice())
		return;

	OwnerAbilitySystemComponent->ApplyModToAttribute(UGHeroAttributeSet::GetGoldAttribute(), EGameplayModOp::Additive, -ItemToPurchase->GetPrice());
	UE_LOG(LogTemp, Warning, TEXT("Buy Item: %s"), *(ItemToPurchase->GetName()))
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
