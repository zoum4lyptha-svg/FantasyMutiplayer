// Fill out your copyright notice in the Description page of Project Settings.


#include "TargetActor_GroundSelect.h"

#include "Grey/Grey.h"


ATargetActor_GroundSelect::ATargetActor_GroundSelect()
{
	PrimaryActorTick.bCanEverTick = true;
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

	return TraceResult.ImpactPoint;
}

