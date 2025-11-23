// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayWidget.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Widgets/ValueGauge.h"
#include "GAS/GAttributeSet.h"


void UGameplayWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// todo: 本工程的ASC是挂在 Character上的，但是多人最佳实践是在playerState上
	OwnerAbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwningPlayerPawn());
	if (OwnerAbilitySystemComponent)
	{
		// 构建 main WBP 时 刷新UI，绑定属性委托
		// 注意：这里构造UI绑定委托的时序在服务器onProcess之后，所以一定会获取到正确的值
		HealthBar->SetAndBoundToGameplayAttribute(OwnerAbilitySystemComponent, UGAttributeSet::GetHealthAttribute(), UGAttributeSet::GetMaxHealthAttribute());
		ManaBar->SetAndBoundToGameplayAttribute(OwnerAbilitySystemComponent, UGAttributeSet::GetManaAttribute(), UGAttributeSet::GetMaxManaAttribute());
	}
}
