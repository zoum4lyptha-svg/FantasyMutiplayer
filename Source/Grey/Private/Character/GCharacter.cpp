// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/GCharacter.h"

#include "Kismet/GameplayStatics.h"
#include "GAS/GAbilitySystemComponent.h"
#include "GAS/GAttributeSet.h"
#include "GAS/GAbilitySystemStatics.h"
#include "Net/UnrealNetwork.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
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

	BindGASChangeDelegates();
}

void AGCharacter::ServerSideInit()
{
	GAbilitySystemComponent->InitAbilityActorInfo(this, this);
	GAbilitySystemComponent->ApplyInitialEffects();  
	GAbilitySystemComponent->GiveInitialAbilities();
}

void AGCharacter::ClientSideInit()
{
	//客户端InitAbility的顺序需要在服务器确认后，否则很容易有问题
	GAbilitySystemComponent->InitAbilityActorInfo(this, this);
	// 客户端不会应用GE,客户端的GE来源于服务器的同步
	
}

bool AGCharacter::IsLocallyControlledByPlayer() const
{
	// 客户端的主控 或 服务器上的客户端主控(划掉)，
	// 换一个内置的接口，别自己抖机灵判断 owner connection了 
	//return GetLocalRole() == ROLE_AutonomousProxy || GetRemoteRole() == ROLE_AutonomousProxy;
	return GetController() && GetController()->IsLocalPlayerController();
}

void AGCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AGCharacter, TeamID);
}


void AGCharacter::BeginPlay()
{
	Super::BeginPlay();

	// 头部组件初始化逻辑是所有 Clients（包括主控，和 模拟 都要走到的）
	ConfigureOverHeadStatusWidget();

	MeshRelativeTransform = GetMesh()->GetRelativeTransform();
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

void AGCharacter::BindGASChangeDelegates()
{
	// 监听 death这个tag的变化
	if (GAbilitySystemComponent)
	{
		GAbilitySystemComponent->RegisterGameplayTagEvent(UGAbilitySystemStatics::GetDeadStatTag()).AddUObject(this, &AGCharacter::DeathTagUpdated);
	}
}

void AGCharacter::DeathTagUpdated(const FGameplayTag Tag, int32 NewCount)
{
	if (NewCount != 0)
	{
		StartDeathSequence();
	}
	else
	{
		Respawn();
	}
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

// 把血条关了
void AGCharacter::SetStatusGaugeEnabled(bool bIsEnabled)
{
	GetWorldTimerManager().ClearTimer(HeadStatGaugeVisibilityUpdateTimerHandle);
	if (bIsEnabled)
	{
		ConfigureOverHeadStatusWidget();
	}
	else
	{
		OverHeadWidgetComponent->SetHiddenInGame(true);
	}
}

void AGCharacter::DeathMontageFinished()
{
	SetRagdollEnabled(true);
}

void AGCharacter::SetRagdollEnabled(bool bIsEnabled)
{
	if (bIsEnabled)
	{
		GetMesh()->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		GetMesh()->SetSimulatePhysics(true);
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	}
	else
	{
		GetMesh()->SetSimulatePhysics(false);
		GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GetMesh()->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
		GetMesh()->SetRelativeTransform(MeshRelativeTransform);
	}
}

void AGCharacter::PlayDeathAnimation()
{
	if (DeathMontage)
	{
		float MontageDuration = PlayAnimMontage(DeathMontage);
		// 不要等动画播完了躺地上再打开布娃娃，不然效果很鬼畜,反复蹦迪.jpg
		GetWorldTimerManager().SetTimer(DeathMontageTimerHandle, this, &AGCharacter::DeathMontageFinished, MontageDuration + DeathMontageFinishTimeShift);
	}

}

void AGCharacter::StartDeathSequence()
{
	// 执行自定义死亡事件（如果是 AI可以在这里关闭行为）--hide角色--禁止PC输入--关闭碰撞
	OnDead();
	PlayDeathAnimation();
	SetStatusGaugeEnabled(false);

	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AGCharacter::Respawn()
{
	OnRespawn();
	// debug: 重生时别忘了关了布娃娃
	SetRagdollEnabled(false);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
	GetMesh()->GetAnimInstance()->StopAllMontages(0.f);
	SetStatusGaugeEnabled(true);

	// 注意 生成actor一定要限定在服务器
	if (HasAuthority() && GetController())
	{
		TWeakObjectPtr<AActor> StartSpot = GetController()->StartSpot;
		if (StartSpot.IsValid())
		{
			SetActorTransform(StartSpot->GetActorTransform());
		}
	}

	if (GAbilitySystemComponent)
	{
		GAbilitySystemComponent->ApplyFullStatEffect();
	}
}

void AGCharacter::OnDead()
{
}

void AGCharacter::OnRespawn()
{
}

void AGCharacter::SetGenericTeamId(const FGenericTeamId& NewTeamID)
{
	TeamID = NewTeamID;
}

FGenericTeamId AGCharacter::GetGenericTeamId() const
{
	return TeamID;
}

