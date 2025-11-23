// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/GCharacter.h"

#include "Kismet/GameplayStatics.h"
#include "GAS/GAbilitySystemComponent.h"
#include "GAS/GAttributeSet.h"
#include "Components/WidgetComponent.h"
#include "Widgets/OverHeadStatsGauge.h"

#include "Components/SkeletalMeshComponent.h"


AGCharacter::AGCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	//把mesh的碰撞关了
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	GAbilitySystemComponent = CreateDefaultSubobject<UGAbilitySystemComponent>("GAbility System Component");
	GAttributeSet = CreateDefaultSubobject<UGAttributeSet>("GAttribute Set");

	OverHeadWidgetComponent = CreateDefaultSubobject<UWidgetComponent>("Over Head Widget Component");
	// 头部血量组件必须要有 transform 的
	OverHeadWidgetComponent->SetupAttachment(GetRootComponent());
}

void AGCharacter::ServerSideInit()
{
	GAbilitySystemComponent->InitAbilityActorInfo(this, this);
	GAbilitySystemComponent->ApplyInitialEffects();
}

void AGCharacter::ClientSideInit()
{
	//客户端InitAbility的顺序需要在服务器确认后，否则很容易有问题
	GAbilitySystemComponent->InitAbilityActorInfo(this, this);
	// 客户端不会应用GE,客户端的GE来源于服务器的同步
	
}

bool AGCharacter::IsLocallyControlledByPlayer() const
{
	// 客户端的主控 或 服务器上的客户端主控
	return GetLocalRole() == ROLE_AutonomousProxy || GetRemoteRole() == ROLE_AutonomousProxy;
	
}


void AGCharacter::BeginPlay()
{
	Super::BeginPlay();

	// 头部组件初始化逻辑是所有 Clients（包括主控，和 模拟 都要走到的）
	ConfigureOverHeadStatusWidget();
}

void AGCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	// 存在AController 但是不是玩家控制器时 服务器初始化 AI 的 info
	if (NewController && !NewController->IsPlayerController())
	{
		ServerSideInit();
	}
}


void AGCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


void AGCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

UAbilitySystemComponent* AGCharacter::GetAbilitySystemComponent() const
{
	return GAbilitySystemComponent;
}

void AGCharacter::ConfigureOverHeadStatusWidget()
{
	if (!OverHeadWidgetComponent)
	{
		return;
	}

	IsPlayerControlled();

	// 玩家自己不显示 头部血量 
	if (IsLocallyControlledByPlayer())
	{
		OverHeadWidgetComponent->SetHiddenInGame(true);


		return;
	}


	UOverHeadStatsGauge* OverheadStatsGuage = Cast<UOverHeadStatsGauge>(OverHeadWidgetComponent->GetUserWidgetObject());
	if (OverheadStatsGuage)
	{
		OverheadStatsGuage->ConfigureWithASC(GetAbilitySystemComponent());
		OverHeadWidgetComponent->SetHiddenInGame(false);

		GetWorldTimerManager().ClearTimer(HeadStatGaugeVisibilityUpdateTimerHandle);
		// 这里开一个循环计时器代替tick，重复检查组件当前距离是否应该可见
		GetWorldTimerManager().SetTimer(HeadStatGaugeVisibilityUpdateTimerHandle, this, &AGCharacter::UpdateHeadGaugeVisibility, HeadStatGaugeVisiblityCheckUpdateGap, true);
	}
}

void AGCharacter::UpdateHeadGaugeVisibility()
{
	APawn* LocalPlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
	if (LocalPlayerPawn)
	{
		// 比较平方是为了防负数
		float DistSquared = FVector::DistSquared(GetActorLocation(), LocalPlayerPawn->GetActorLocation());
		OverHeadWidgetComponent->SetHiddenInGame(DistSquared > HeadStatGaugeVisiblityRangeSquared);
	}
}

