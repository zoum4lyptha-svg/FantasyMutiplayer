// Fill out your copyright notice in the Description page of Project Settings.


#include "TargetActor_GroundSelect.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "GenericTeamAgentInterface.h"
#include "Abilities/GameplayAbility.h"
#include "Components/DecalComponent.h"
#include "Engine/OverlapResult.h"
#include "Grey/Grey.h"


ATargetActor_GroundSelect::ATargetActor_GroundSelect()
{
	PrimaryActorTick.bCanEverTick = true;
	
	SetRootComponent(CreateDefaultSubobject<USceneComponent>("Root Comp"));

	DecalComp = CreateDefaultSubobject<UDecalComponent>("Decal Comp");
	DecalComp->SetupAttachment(GetRootComponent());
}

void ATargetActor_GroundSelect::ConfirmTargetingAndContinue()
{
	TArray<FOverlapResult> OverlapResults;

	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECC_Pawn);

	FCollisionShape CollisionShape;
	CollisionShape.SetSphere(TargetAreaRadius);

	// 以碰撞球为范围查pawn类型 
	GetWorld()->OverlapMultiByObjectType(OverlapResults,
		GetActorLocation(),
		FQuat::Identity,
		ObjectQueryParams,
		CollisionShape
	);

	// TSet去除重复
	TSet<AActor*> TargetActors;

	// 处理碰撞对象的敌我
	IGenericTeamAgentInterface* OwnerTeamInterface = nullptr; ;
	if (OwningAbility)
	{
		OwnerTeamInterface = Cast<IGenericTeamAgentInterface>(OwningAbility->GetAvatarActorFromActorInfo());
	}

	for (const FOverlapResult& OverlapResult : OverlapResults)
	{
		if (OwnerTeamInterface && OwnerTeamInterface->GetTeamAttitudeTowards(*OverlapResult.GetActor()) == ETeamAttitude::Friendly && !bShouldTargetFriendly)
			continue;

		if (OwnerTeamInterface && OwnerTeamInterface->GetTeamAttitudeTowards(*OverlapResult.GetActor()) == ETeamAttitude::Hostile && !bShouldTargetEnemy)
			continue;

		TargetActors.Add(OverlapResult.GetActor());
	}

	FGameplayAbilityTargetDataHandle TargetData = UAbilitySystemBlueprintLibrary::AbilityTargetDataFromActorArray(TargetActors.Array(), false);

	
	
	// targetData 多传一个 碰撞点data过去
	FGameplayAbilityTargetData_SingleTargetHit* HitLoc = new FGameplayAbilityTargetData_SingleTargetHit;
	HitLoc->HitResult.ImpactPoint = GetActorLocation();

	TargetData.Add(HitLoc);

	
	TargetDataReadyDelegate.Broadcast(TargetData);
}

void ATargetActor_GroundSelect::SetTargetOptions(bool bTargetFriendly, bool bTargetEnenmy)
{
	bShouldTargetFriendly = bTargetFriendly;
	bShouldTargetEnemy = bTargetEnenmy;
}


void ATargetActor_GroundSelect::SetTargetAreaRadius(float NewRadius)
{
	TargetTraceRange = NewRadius;
	// 贴花大小尽量和 选择区域大小保持一致
	DecalComp->DecalSize = FVector{NewRadius};
}


void ATargetActor_GroundSelect::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	// GetTargetPoint 当前视线选点是本地行为，单纯的视觉效果,不经过服务器的。
	// 只有当确认confirm触发委托后，TargetDataHandle内部会RPC给服务器
	if (PrimaryPC && PrimaryPC->IsLocalPlayerController())
	{
		SetActorLocation(GetTargetPoint());
	}
}

FVector ATargetActor_GroundSelect::GetTargetPoint() const
{
	if (!PrimaryPC || !PrimaryPC->IsLocalPlayerController())
		return GetActorLocation();

	FHitResult TraceResult;

	FVector ViewLoc;
	FRotator ViewRot;

	PrimaryPC->GetPlayerViewPoint(ViewLoc, ViewRot);

	// 限定了视线检测的最大距离
	FVector TraceEnd = ViewLoc + ViewRot.Vector() * TargetTraceRange;

	GetWorld()->LineTraceSingleByChannel(TraceResult, ViewLoc, TraceEnd, ECC_Target);

	if (!TraceResult.bBlockingHit)
	{
		GetWorld()->LineTraceSingleByChannel(TraceResult, TraceEnd, TraceEnd + FVector::DownVector * TNumericLimits<float>::Max(), ECC_Target);
	}

	if (!TraceResult.bBlockingHit)
	{
		return GetActorLocation();
	}

	if (bShouldDrawDebug)
	{
		DrawDebugSphere(GetWorld(), TraceResult.ImpactPoint, TargetAreaRadius, 32, FColor::Red);
	}

	
	return TraceResult.ImpactPoint;
}

