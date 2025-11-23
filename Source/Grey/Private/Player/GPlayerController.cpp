// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/GPlayerController.h"
#include "Widgets/GameplayWidget.h"

#include "Player/GPlayerCharacter.h"

void AGPlayerController::OnPossess(APawn* NewPawn)
{
	Super::OnPossess(NewPawn);
	GPlayerCharacter = Cast<AGPlayerCharacter>(NewPawn);
	if (GPlayerCharacter)
	{
		GPlayerCharacter->ServerSideInit();
	}
}

void AGPlayerController::AcknowledgePossession(APawn* NewPawn)
{
	Super::AcknowledgePossession(NewPawn);
	GPlayerCharacter = Cast<AGPlayerCharacter>(NewPawn);
	if (GPlayerCharacter)
	{
		GPlayerCharacter->ClientSideInit();
		// 注意服务器是不画UI的，所以在这里spawn UI ，
		// 而且此时 服务器已经初始化 ASC完成，所以客户端不需要额外的rpc 去刷新数据
		SpawnGameplayWidget();
	}
}

void AGPlayerController::SpawnGameplayWidget()
{
	// 兜底，1.模拟 proxy不画 UI 2.敌人角色不画 UI  只在当前主控角色画UI
	if (!IsLocalPlayerController())
		return;

	GameplayWidget = CreateWidget<UGameplayWidget>(this, GameplayWidgetClass);
	if (GameplayWidget)
	{
		GameplayWidget->AddToViewport();
	}
}
