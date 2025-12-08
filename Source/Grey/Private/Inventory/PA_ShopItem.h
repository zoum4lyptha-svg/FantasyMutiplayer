// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PA_ShopItem.generated.h"


class UPA_ShopItem;
USTRUCT(BlueprintType)
struct FItemCollection
{
	GENERATED_BODY()
public:
	FItemCollection();
	FItemCollection(const TArray<const UPA_ShopItem*>& InItems);
	void AddItem(const UPA_ShopItem* NewItem, bool bAddUnique = false);
	bool Contains(const UPA_ShopItem* Item) const;
	const TArray<const UPA_ShopItem*>& GetItems() const;

private:
	TArray<const UPA_ShopItem*> Items;
};
/**
 * 
 */
UCLASS()
class UPA_ShopItem : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	// asset的 id 一般等于 类型 + 个性化名字，id是网络同步，资源管理等中的唯一标识
	virtual FPrimaryAssetId GetPrimaryAssetId() const override;
	// Get type 静态是为了不实例化ShopItem也能拿到类型
	static FPrimaryAssetType GetShopItemAssetType();
	
	
	UTexture2D* GetIcon() const;
	FText GetItemName() const { return ItemName; }
	FText GetItemDescription() const { return ItemDescription; }
	float GetPrice() const { return Price; }
	float GetSellPrice() const { return Price / 2.f; }

	TSubclassOf<class UGameplayEffect> GetEquippedEffect() const { return EquippedEffect; }
	TSubclassOf<class UGameplayEffect> GetConsumeEffect() const { return ConsumeEffect; }
	TSubclassOf<class UGameplayAbility> GetGrantedAbility() const { return GrantedAbility; }
	bool GetIsStackable() const { return bIsStackable; }
	bool GetIsConsumable() const { return bIsConsumable; }
	int GetMaxStackCount() const { return MaxStackCount; }
	const TArray<TSoftObjectPtr<UPA_ShopItem>>& GetIngredients() const { return IngredientItems; }

private:
	// icon 暂定软引用，按需加载
	UPROPERTY(EditDefaultsOnly, Category = "ShopItem")
	TSoftObjectPtr<UTexture2D> Icon;

	UPROPERTY(EditDefaultsOnly, Category = "ShopItem")
	float Price;

	UPROPERTY(EditDefaultsOnly, Category = "ShopItem")
	FText ItemName;

	UPROPERTY(EditDefaultsOnly, Category = "ShopItem")
	FText ItemDescription;

	UPROPERTY(EditDefaultsOnly, Category = "ShopItem")
	bool bIsConsumable;

	// 装备效果GE
	UPROPERTY(EditDefaultsOnly, Category = "ShopItem")
	TSubclassOf<class UGameplayEffect> EquippedEffect;

	// 消耗品效果GE
	UPROPERTY(EditDefaultsOnly, Category = "ShopItem")
	TSubclassOf<class UGameplayEffect> ConsumeEffect;

	UPROPERTY(EditDefaultsOnly, Category = "ShopItem")
	TSubclassOf<class UGameplayAbility> GrantedAbility;

	UPROPERTY(EditDefaultsOnly, Category = "ShopItem")
	bool bIsStackable = false;

	UPROPERTY(EditDefaultsOnly, Category = "ShopItem")
	int MaxStackCount = 5;

	// 合成配方 这个data可能很大的，也是软引用按需加载
	UPROPERTY(EditDefaultsOnly, Category = "ShopItem")
	TArray<TSoftObjectPtr<UPA_ShopItem>> IngredientItems;
};