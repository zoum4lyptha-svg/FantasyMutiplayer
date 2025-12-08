// Fill out your copyright notice in the Description page of Project Settings.


#include "ShopItemWidget.h"

#include "Inventory/PA_ShopItem.h"

// list view add item 回调
void UShopItemWidget::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);

	// 外部listview additem 时 将准备的资产 cast成 ShopItem类，随后执行内部初始化
	ShopItem = Cast<UPA_ShopItem>(ListItemObject);
	if (!ShopItem)
	{
		return;
	}

	SetIcon(ShopItem->GetIcon());
}
