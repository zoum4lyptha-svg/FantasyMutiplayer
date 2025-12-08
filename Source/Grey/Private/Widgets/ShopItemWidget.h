// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemWidget.h"
#include "Inventory/PA_ShopItem.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "ShopItemWidget.generated.h"

/**
 *    设计上ShopItem 将会被add到 listview排布，所以要继承IUserObjectListEntry
 */
UCLASS()
class GREY_API UShopItemWidget : public UItemWidget, public IUserObjectListEntry
{
	GENERATED_BODY()
public:
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
	FORCEINLINE const UPA_ShopItem* GetShopItem() const { return ShopItem; }
private:
	UPROPERTY()
	const UPA_ShopItem* ShopItem;
};
