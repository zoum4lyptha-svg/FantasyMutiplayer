// Fill out your copyright notice in the Description page of Project Settings.


#include "Animations/GAnimInstance.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GAS/GAbilitySystemStatics.h"
#include "Kismet/KismetMathLibrary.h"

void UGAnimInstance::NativeInitializeAnimation()
{
	// player 或 AI 角色
	OwnerCharacter = Cast<ACharacter>(TryGetPawnOwner());
	if (OwnerCharacter)
	{
		OwnerMovementComp = OwnerCharacter->GetCharacterMovement();
	}
	
	// anim instance 监听 stats.aim tag，更新 bIsAimming,这个 bool 在动画蓝图中播放 用于在静止状态下瞄准时的动画
	UAbilitySystemComponent* OwnerASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TryGetPawnOwner());
	if (OwnerASC)
	{
		OwnerASC->RegisterGameplayTagEvent(UGAbilitySystemStatics::GetAimStatTag()).AddUObject(this, &UGAnimInstance::OwnerAimTagChanged);
	}
}

void UGAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	if (OwnerCharacter)
	{
		FVector Velocity = OwnerCharacter->GetVelocity();
		Speed = Velocity.Length();


		//简单的求差值，计算yaw角速度，代表旋转有多快
		// 注意这个是瞬时的，可能会比较快，需要平滑一下
		FRotator BodyRot = OwnerCharacter->GetActorRotation();
		FRotator BodyRotDelta = UKismetMathLibrary::NormalizedDeltaRotator(BodyRot, BodyPrevRot);
		BodyPrevRot = BodyRot;

		YawSpeed = BodyRotDelta.Yaw / DeltaSeconds;
		SmoothedYawSpeed = UKismetMathLibrary::FInterpTo(SmoothedYawSpeed, YawSpeed, DeltaSeconds, YawSpeedSmoothLerpSpeed);
		
		FRotator ControlRot = OwnerCharacter->GetBaseAimRotation();
		// 算相机朝向和人体朝向的Delta
		LookRotOffset = UKismetMathLibrary::NormalizedDeltaRotator(ControlRot, BodyRot);

		FwdSpeed = Velocity.Dot(ControlRot.Vector());
		RightSpeed = Velocity.Dot(ControlRot.Vector().Cross(FVector::UpVector));
	}

	if (OwnerMovementComp)
	{
		bIsJumping = OwnerMovementComp->IsFalling();
	}

}

void UGAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{

}

bool UGAnimInstance::ShouldDoFullBody() const
{
	return (GetSpeed() <= 0) && !(GetIsAimming());
}

void UGAnimInstance::OwnerAimTagChanged(const FGameplayTag Tag, int32 NewCount)
{
	bIsAimming = NewCount != 0;
}
