// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/GPlayerCharacter.h"

#include "Camera/CameraComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"

AGPlayerCharacter::AGPlayerCharacter()
{

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>("Camera Boom");
	CameraBoom->SetupAttachment(GetRootComponent());
	// 相机旋转跟随pawn,拓展视角
	CameraBoom->bUsePawnControlRotation = true;


	ViewCam = CreateDefaultSubobject<UCameraComponent>("View Cam");
	ViewCam->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);

	// 关闭角色跟随视角旋转，只通过输入控制
	bUseControllerRotationYaw = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 720.f, 0.f);
	

}


void AGPlayerCharacter::PawnClientRestart()
{
	Super::PawnClientRestart();
	APlayerController* OwningPlayerController = GetController<APlayerController>();
	if (OwningPlayerController)
	{
		UEnhancedInputLocalPlayerSubsystem* InputSubsystem = OwningPlayerController->GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
		if (InputSubsystem)
		{
			// 切换pawn时，先卸载旧的输入映射，再加载新的pawn 的映射
			InputSubsystem->RemoveMappingContext(GameplayInputMappingContext);
			InputSubsystem->AddMappingContext(GameplayInputMappingContext, 0);	
		}
	}
}

void AGPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	UEnhancedInputComponent* EnhancedInputComp = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (EnhancedInputComp)
	{
		EnhancedInputComp->BindAction(JumpInputAction, ETriggerEvent::Triggered, this, &AGPlayerCharacter::Jump);
		EnhancedInputComp->BindAction(LookInputAction, ETriggerEvent::Triggered, this, &AGPlayerCharacter::HandleLookInput);
		EnhancedInputComp->BindAction(MoveInputAction, ETriggerEvent::Triggered, this, &AGPlayerCharacter::HandleMoveInput);
	}
}

void AGPlayerCharacter::HandleLookInput(const FInputActionValue& InputActionValue)
{
	// 获取鼠标输入值（Vector2D: X=水平, Y=垂直）
	FVector2D InputVal = InputActionValue.Get<FVector2D>();

	// 视角移动
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		AddControllerPitchInput(-InputVal.Y);
		AddControllerYawInput(InputVal.X);
	}
}



void AGPlayerCharacter::HandleMoveInput(const FInputActionValue& InputActionValue)
{
	FVector2D InputVal = InputActionValue.Get<FVector2D>();
	// 归一化输入值,否则移动会快一点
	InputVal.Normalize();
	
	// 移动方向 = 前方向 * 输入 + 右方向 * 输入
	// 这个是适合俯视角的移动方向
	AddMovementInput(GetMoveFwdDir()*InputVal.Y + GetLookRightDir() * InputVal.X);
}

FVector AGPlayerCharacter::GetLookRightDir() const
{
	return ViewCam->GetRightVector();
}

FVector AGPlayerCharacter::GetLookFwdDir() const
{
	return ViewCam->GetForwardVector();
}

FVector AGPlayerCharacter::GetMoveFwdDir() const
{
	// 其实也可以去除Z轴，然后归一化
	return FVector::CrossProduct(GetLookRightDir(), FVector::UpVector);
}
