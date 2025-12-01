// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "GameplayTagContainer.h"
#include "GAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class UGAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:	

	// beginplay
	virtual void NativeInitializeAnimation() override;
	
	// tick
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	
	// safe tick
	virtual void NativeThreadSafeUpdateAnimation(float DeltaSeconds) override;


	// 为了网络同步的方便，送给动画状态机的变量（如果不是直接从 movementcomponent 拿），尽量在C++写
	UFUNCTION(BlueprintCallable, meta=(BlueprintThreadSafe))
	FORCEINLINE float GetSpeed() const { return Speed; }

	UFUNCTION(BlueprintCallable, meta=(BlueprintThreadSafe))
	FORCEINLINE bool IsMoving() const { return Speed != 0; }

	UFUNCTION(BlueprintCallable, meta=(BlueprintThreadSafe))
	FORCEINLINE bool IsNotMoving() const { return Speed == 0; }

	UFUNCTION(BlueprintCallable, meta=(BlueprintThreadSafe))
	FORCEINLINE float GetYawSpeed() const { return YawSpeed; }

	UFUNCTION(BlueprintCallable, meta=(BlueprintThreadSafe))
	FORCEINLINE float GetFwdSpeed() const { return FwdSpeed; }

	UFUNCTION(BlueprintCallable, meta=(BlueprintThreadSafe))
	FORCEINLINE float GetRightSpeed() const { return RightSpeed; }

	
	UFUNCTION(BlueprintCallable, meta=(BlueprintThreadSafe))
	FORCEINLINE float GetSmoothedYawSpeed() const { return SmoothedYawSpeed; }
	UFUNCTION(BlueprintCallable, meta=(BlueprintThreadSafe))
	FORCEINLINE bool GetIsJumping() const { return bIsJumping; }

	UFUNCTION(BlueprintCallable, meta=(BlueprintThreadSafe))
	FORCEINLINE bool GetIsOnGround() const { return !bIsJumping; }

	UFUNCTION(BlueprintCallable, meta=(BlueprintThreadSafe))
	FORCEINLINE bool GetIsAimming() const { return bIsAimming; }
	
	UFUNCTION(BlueprintCallable, meta=(BlueprintThreadSafe))
	FORCEINLINE float GetLookYawOffset() const { return LookRotOffset.Yaw; }

	//todo: 这里其实有BUG,在多人网络时，GetBaseAimRotation 这个接口拿到的引擎管线的pitch存在着不支持同步的问题
	UFUNCTION(BlueprintCallable, meta=(BlueprintThreadSafe))
	FORCEINLINE float GetLookPitchOffset() const { return LookRotOffset.Pitch; }

	UFUNCTION(BlueprintCallable, meta=(BlueprintThreadSafe))
	bool ShouldDoFullBody() const;

private:
	
	void OwnerAimTagChanged(const FGameplayTag Tag, int32 NewCount);
	
	UPROPERTY()
	class ACharacter* OwnerCharacter;

	UPROPERTY()
	class UCharacterMovementComponent* OwnerMovementComp;


	float Speed;

	float SmoothedYawSpeed;

	// 这里新加的所有单位都是从 GAS 或者 movement组件里拿到的，不需要多写同步
	float FwdSpeed;
	float RightSpeed;
	bool bIsAimming;
	
	float YawSpeed;

	bool bIsJumping;

	FRotator LookRotOffset;

	UPROPERTY(EditAnywhere, Category = "Animation")
	float YawSpeedSmoothLerpSpeed = 1.f;

	FRotator BodyPrevRot;
};
