// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Inventory/PA_ShopItem.h"
#include "ItemToolTip.generated.h"

/**
 * 
 */
UCLASS()
class GREY_API UItemToolTip : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void SetItem(const UPA_ShopItem* Item);
	void SetPrice(float newPrice);
private:
	UPROPERTY(meta=(BindWidget))
	class UImage* IconImage;

	UPROPERTY(meta=(BindWidget))
	class UTextBlock* ItemTitleText;

	UPROPERTY(meta=(BindWidget))
	class UTextBlock* ItemDescriptionText;

	UPROPERTY(meta=(BindWidget))
	class UTextBlock* ItemPriceText;
};
