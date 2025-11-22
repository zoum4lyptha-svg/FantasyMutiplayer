// Fill out your copyright notice in the Description page of Project Settings.


#include "Animations/GAnimInstance.h"

#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UGAnimInstance::NativeInitializeAnimation()
{
	// player 或 AI 角色
	OwnerCharacter = Cast<ACharacter>(TryGetPawnOwner());
	if (OwnerCharacter)
	{
		OwnerMovementComp = OwnerCharacter->GetCharacterMovement();
	}
}

void UGAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	if (OwnerCharacter)
	{
		Speed = OwnerCharacter->GetVelocity().Length();


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

	}

	if (OwnerMovementComp)
	{
		bIsJumping = OwnerMovementComp->IsFalling();
	}

}

void UGAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{

}