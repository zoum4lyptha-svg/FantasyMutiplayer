// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemWidget.h"
#include "Inventory/PA_ShopItem.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "ShopItemWidget.generated.h"

class UShopItemWidget;


DECLARE_MULTICAST_DELEGATE_OneParam(FOnItemPurchaseIssused, const UPA_ShopItem*);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnShopItemSelected, const UShopItemWidget*);

/**
 *    设计上ShopItem 将会被add到 listview排布，所以要继承IUserObjectListEntry
 */
UCLASS()
class GREY_API UShopItemWidget : public UItemWidget, public IUserObjectListEntry
{
	GENERATED_BODY()
public:
	FOnItemPurchaseIssused OnItemPurchaseIssued;
	FOnShopItemSelected OnShopItemClicked;
	
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
	FORCEINLINE const UPA_ShopItem* GetShopItem() const { return ShopItem; }
private:
	UPROPERTY()
	const UPA_ShopItem* ShopItem;
	
	// 购买 item的委托(暂定为右键点击触发) && 选中WBP的委托（暂定左键点击触发）
	virtual void RightButtonClicked() override;
	virtual void LeftButtonClicked() override;
};
