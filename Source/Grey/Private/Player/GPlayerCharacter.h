// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/GCharacter.h"
#include "GAS/GGameplayAbilityTypes.h"
#include "InputActionValue.h"
#include "GPlayerCharacter.generated.h"

/**
 * 
 */
UCLASS()
class AGPlayerCharacter : public AGCharacter
{
	GENERATED_BODY()

public:

	AGPlayerCharacter();

	// 客户端的pawn 时，重新添加输入映射
	virtual void PawnClientRestart() override;

	// 设置玩家增强输入绑定
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:

	/*************************************************************/
	/*                         3C -- 相机                         */
	/*************************************************************/
	UPROPERTY(VisibleDefaultsOnly, Category = "View")
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleDefaultsOnly, Category = "View")
	class UCameraComponent* ViewCam;
	
	UPROPERTY(EditDefaultsOnly, Category = view)
	FVector CameraAimLocalOffset;

	UPROPERTY(EditDefaultsOnly, Category = view)
	float CamerLerpSpeed = 20.f;
	
	FTimerHandle CamerLerpTimerHandle;

	// 获取右方向，前方向，移动方向（不含Z轴）
	FVector GetLookRightDir() const;
	FVector GetLookFwdDir() const;
	FVector GetMoveFwdDir() const;
	

	void LerpCameraToLocalOffsetLocation(const FVector& Goal);
	void TickCameraLocalOffsetLerp(FVector Goal);

	/*************************************************************/
	/*                       Gameplay Ability                    */
	/*************************************************************/
private:
	virtual void OnAimStateChanged(bool bIsAimming) override;
	
	UPROPERTY()
	class UGHeroAttributeSet* HeroAttributeSet;

	/*************************************************************/
	/*                           Input                           */
	/*************************************************************/
private:

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* JumpInputAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* LookInputAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* MoveInputAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputMappingContext* GameplayInputMappingContext;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TMap<EGAbilityInputID, class UInputAction*> GameplayAbilityInputActions;

	void HandleLookInput(const FInputActionValue& InputActionValue);
	
	void HandleMoveInput(const FInputActionValue& InputActionValue);

	void HandleAbilityInput(const FInputActionValue& InputActionValue, EGAbilityInputID InputID);

	void SetInputEnabledFromPlayerController(bool bEnabled);
	/*************************************************************/
	/*                           Stun                            */
	/*************************************************************/
	virtual void OnStun() override;
	virtual void OnRecoverFromStun() override;




	/*************************************************************/
	/*                      Death and Respawn                    */
	/*************************************************************/

	//自定义死亡重生逻辑
	virtual void OnDead() override;
	virtual void OnRespawn() override;



};
