// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/GCharacter.h"
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

	//3C组件
	UPROPERTY(VisibleDefaultsOnly, Category = "View")
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleDefaultsOnly, Category = "View")
	class UCameraComponent* ViewCam;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* JumpInputAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* LookInputAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* MoveInputAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputMappingContext* GameplayInputMappingContext;

	void HandleLookInput(const FInputActionValue& InputActionValue);
	
	void HandleMoveInput(const FInputActionValue& InputActionValue);

	// 获取右方向，前方向，移动方向（不含Z轴）
	FVector GetLookRightDir() const;
	FVector GetLookFwdDir() const;
	FVector GetMoveFwdDir() const;
};
