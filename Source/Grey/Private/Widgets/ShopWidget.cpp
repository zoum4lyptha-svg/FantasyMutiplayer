// Fill out your copyright notice in the Description page of Project Settings.


#include "ShopWidget.h"

#include "ShopItemWidget.h"
#include "Components/TileView.h"
#include "FeameWork/GAssetManager.h"

void UShopWidget::NativeConstruct()
{
	Super::NativeConstruct();
	SetIsFocusable(true);
	LoadShopItems();
	//构建UI完成后 保存  ShopItem -> ItemWBP 的 映射
	ShopItemList->OnEntryWidgetGenerated().AddUObject(this, &UShopWidget::ShopItemWidgetGenerated);
}

void UShopWidget::LoadShopItems()
{
	//异步加载 ShopItems
	UGAssetManager::Get().LoadShopItems(FStreamableDelegate::CreateUObject(this, &UShopWidget::ShopItemLoadFinished));
}

void UShopWidget::ShopItemLoadFinished()
{
	TArray<const UPA_ShopItem*> ShopItems;
	UGAssetManager::Get().GetLoadedShopItems(ShopItems);
	for (const UPA_ShopItem* ShopItem : ShopItems)
	{
		// 加入listview UI排布（这里回调会执行item内部的初始逻辑）
		ShopItemList->AddItem(const_cast<UPA_ShopItem*>(ShopItem));
	}
}

void UShopWidget::ShopItemWidgetGenerated(UUserWidget& NewWidget)
{
	UShopItemWidget* ItemWidget = Cast<UShopItemWidget>(&NewWidget);
	if (ItemWidget)
	{
		ItemsMap.Add(ItemWidget->GetShopItem(), ItemWidget);
	}
}
