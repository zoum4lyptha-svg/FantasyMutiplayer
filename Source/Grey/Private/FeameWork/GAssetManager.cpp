// Fill out your copyright notice in the Description page of Project Settings.


#include "GAssetManager.h"

#include "Inventory/PA_ShopItem.h"

UGAssetManager& UGAssetManager::Get()
{
	UGAssetManager* Singleton = Cast<UGAssetManager>(GEngine->AssetManager.Get());
	if (Singleton)
	{
		return *Singleton;
	}

	UE_LOG(LogLoad, Fatal, TEXT("Asset Manager Needs to be of the type GAssetMaanger"));
	return (*NewObject<UGAssetManager>());
}

void UGAssetManager::LoadShopItems(const FStreamableDelegate& LoadFinishedCallback)
{
	// 注意 LoadPrimaryAssetsWithType 内部是引擎帮你写好的异步加载，以及完成回调
	LoadPrimaryAssetsWithType(UPA_ShopItem::GetShopItemAssetType(), TArray<FName>(), FStreamableDelegate::CreateUObject(this, &UGAssetManager::ShopItemLoadFinished, LoadFinishedCallback));
}

//  after LoadShopItems and get 
bool UGAssetManager::GetLoadedShopItems(TArray<const UPA_ShopItem*>& OutItems) const
{
	TArray<UObject*> LoadedObjects;
	bool bLoaded = GetPrimaryAssetObjectList(UPA_ShopItem::GetShopItemAssetType(), LoadedObjects);

	if (bLoaded)
	{
		for (UObject* ObjectLoaded : LoadedObjects)
		{
			OutItems.Add(Cast<UPA_ShopItem>(ObjectLoaded));
		}
	}

	return bLoaded;
}

void UGAssetManager::ShopItemLoadFinished(FStreamableDelegate Callback)
{
	//异步加载完成，触发上层委托的LoadFinishedCallback回调
	Callback.ExecuteIfBound();
	// item异步加载完成后 根据 Shop item内部的合成关系构建 原料-产物 映射，方便后续查合成表
	BuildItemMaps();
}

void UGAssetManager::BuildItemMaps()
{
	TArray<const UPA_ShopItem*> LoadedItems;
	// 注意 此时 icon等 软引用资源 是没有load的，在这里加载使用是会阻塞线程的
	if (GetLoadedShopItems(LoadedItems))
	{
		for (const UPA_ShopItem* Item : LoadedItems)
		{
			if (Item->GetIngredients().Num() == 0)
			{
				continue;
			}

			TArray<const UPA_ShopItem*> Items;
			for (const TSoftObjectPtr<UPA_ShopItem>& Ingredient : Item->GetIngredients())
			{
				UPA_ShopItem* IngredientItem = Ingredient.LoadSynchronous();
				Items.Add(IngredientItem);
				// 构建 材料-产物 的正逆向映射 
				// IngredientMap[铁剑] = [铁矿石, 木板]  
				// CombinationMap[铁矿石] = [铁箭, 铁剑 ]
				AddToCombinationMap(IngredientItem, Item);
			}

			IngredientMap.Add(Item, FItemCollection{Items});
		}
	}
}

void UGAssetManager::AddToCombinationMap(const UPA_ShopItem* Ingredient, const UPA_ShopItem* CombinationItem)
{
	FItemCollection* Combinations = CombinationMap.Find(Ingredient);
	if (Combinations)
	{
		if (!Combinations->Contains(CombinationItem))
			CombinationMap.Add(CombinationItem);
	}
	else
	{
		CombinationMap.Add(Ingredient, FItemCollection{TArray<const UPA_ShopItem*>{CombinationItem}});
	}
}
