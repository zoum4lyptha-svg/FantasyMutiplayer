// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ItemWidget.generated.h"

/**
 * 背包和库存的基类
 */
UCLASS()
class GREY_API UItemWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;
	virtual void SetIcon(UTexture2D* IconTexture);
private:
	UPROPERTY(meta=(BindWidget))
	class UImage* ItemIcon;


	// 鼠标点击和释放事件
	virtual FReply NativeOnMouseButtonDown( const FGeometry& InGeometry, const FPointerEvent& InMouseEvent ) override;
	virtual FReply NativeOnMouseButtonUp( const FGeometry& InGeometry, const FPointerEvent& InMouseEvent ) override;

	virtual void RightButtonClicked();
	virtual void LeftButtonClicked();
};
