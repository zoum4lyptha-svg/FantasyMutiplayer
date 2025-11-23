// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class AGPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	// 引擎管线 限定的服务器执行的函数，顺序在process之后
	void OnPossess(APawn* NewPawn) override;
	
	// 管线限定的客户端执行函数， 顺序是 服务器执行 process-- 网络同步给客户端 --- 客户端执行AcknowledgePossession
	// 一般用于以正确的时序初始化 本地的ASC等数据
	void AcknowledgePossession(APawn* NewPawn) override;
private:

	void SpawnGameplayWidget();
	
	UPROPERTY()
	class AGPlayerCharacter* GPlayerCharacter;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<class UGameplayWidget> GameplayWidgetClass;

	UPROPERTY()
	class UGameplayWidget* GameplayWidget;

	
	
};
