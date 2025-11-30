// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "Blueprint/UserWidget.h"
#include "AbilityGauge.generated.h"

/**
 *   虚幻神人设计之不带 IUserObjectListEntry 这个接口是不给你配 UListView表的
 */

USTRUCT(BlueprintType)
struct FAbilityWidgetData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class UGameplayAbility> AbilityClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName AbilityName;

	// 这里纹理用TSoftObjectPtr，用到时才会加载，节约内存
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UTexture2D> Icon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Description;
};

UCLASS()
class GREY_API UAbilityGauge : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;
	// 这个会在标记为IUserObjectListEntry的item 添加进list（additem）执行结束后调用
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
	
	void ConfigureWithWidgetData(const FAbilityWidgetData* WidgetData);
private:
	UPROPERTY(EditDefaultsOnly, Category = "Visual")
	FName IconMaterialParamName = "Icon";
	
	UPROPERTY(meta=(BindWidget))
	class UImage* Icon;

	UPROPERTY(meta=(BindWidget))
	class UTextBlock* CooldownCounterText;

	UPROPERTY(meta=(BindWidget))
	class UTextBlock* CooldownDurationText;

	UPROPERTY(meta=(BindWidget))
	class UTextBlock* CostText;
	
	UPROPERTY()
	class UGameplayAbility* AbilityCDO;
};
