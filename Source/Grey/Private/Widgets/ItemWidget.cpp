// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemWidget.h"

#include "Components/Image.h"

void UItemWidget::NativeConstruct()
{
	Super::NativeConstruct();
	// 这个容易忘记，设置为鼠标可聚焦，才能响应鼠标事件
	SetIsFocusable(true);
}

void UItemWidget::SetIcon(UTexture2D* IconTexture)
{
	ItemIcon->SetBrushFromTexture(IconTexture);
}

FReply UItemWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FReply SuperReply = Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
	if (InMouseEvent.IsMouseButtonDown(EKeys::RightMouseButton))
	{
		return FReply::Handled().SetUserFocus(TakeWidget());
	}

	if (InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
	{
		// 左键额外有拖拽检查
		return FReply::Handled().SetUserFocus(TakeWidget()).DetectDrag(TakeWidget(), EKeys::LeftMouseButton);
	}

	return SuperReply;
}

FReply UItemWidget::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FReply SuperReply = Super::NativeOnMouseButtonUp(InGeometry, InMouseEvent);

	if (HasAnyUserFocus())
	{
		if (InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
		{
			// 按下设置WBP焦点,松开触发点击事件
			RightButtonClicked();
			return FReply::Handled();
		}
		if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
		{
			LeftButtonClicked();
			return FReply::Handled();
		}
	}

	return SuperReply;
}

void UItemWidget::RightButtonClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("Right mouse button clicked"));
}

void UItemWidget::LeftButtonClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("Left mouse button clicked"));
}
