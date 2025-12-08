// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "Inventory/PA_ShopItem.h"
#include "GAssetManager.generated.h"

/**
 * 注意 管理资源的 AssetManager是一个全局单例
 */
UCLASS()
class GREY_API UGAssetManager : public UAssetManager
{
	GENERATED_BODY()
	
public:
	static UGAssetManager& Get();
	
	// 注意 AssetManager 加载的Items应该作为 DataAsset（定义），不是实例！ 不要修改数据,最好用 const 做好保护
	// LoadShopItems 是异步逻辑，不过引擎帮你写好了，不用自己写 AsyncTask
	void LoadShopItems(const FStreamableDelegate& LoadFinishedCallback);
	bool GetLoadedShopItems(TArray<const UPA_ShopItem*>& OutItems) const;
private:
	void ShopItemLoadFinished(FStreamableDelegate Callback);
	void BuildItemMaps();
	void AddToCombinationMap(const UPA_ShopItem* Ingredient, const UPA_ShopItem* CombinationItem);

	// 制作表 / 合成表
	UPROPERTY()
	TMap<const UPA_ShopItem*, FItemCollection> CombinationMap;
	UPROPERTY()
	TMap<const UPA_ShopItem*, FItemCollection> IngredientMap;
};
