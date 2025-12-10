// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "InventoryItem.generated.h"


class UPA_ShopItem;


USTRUCT()
struct FInventoryItemHandle
{
	GENERATED_BODY()
public:
	// 构建时 默认构造一个无效的handle,只有通过CreateHandle才可以取到有效handle
	FInventoryItemHandle();
	static FInventoryItemHandle InvalidHandle();
	static FInventoryItemHandle CreateHandle();

	bool IsValid() const;
	uint32 GetHandleId() const { return HandleId; }
private:
	explicit FInventoryItemHandle(uint32 Id);

	UPROPERTY()
	uint32 HandleId;

	static uint32 GenerateNextId();
	static uint32 GetInvalidId();
};

bool operator==(const FInventoryItemHandle& Lhs, const FInventoryItemHandle& Rhs);
uint32 GetTypeHash(const FInventoryItemHandle& Key);

/**
 * 
 */
UCLASS()
class GREY_API UInventoryItem : public UObject
{
	GENERATED_BODY()
	
	public:
    	void InitItem(const FInventoryItemHandle& NewHandle, const UPA_ShopItem* NewShopItem);
    	const UPA_ShopItem* GetShopItem() const { return ShopItem; }
    	FInventoryItemHandle GetHandle() const { return Handle; }
    private:
		// 仓库物品本身也存了一份handle
    	UPROPERTY()
    	const UPA_ShopItem* ShopItem;
    	FInventoryItemHandle Handle;
};
