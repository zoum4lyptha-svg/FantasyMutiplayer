// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/GPlayerCharacter.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GAS/GAbilitySystemStatics.h"
#include "GAS/GHeroAttributeSet.h"
#include "Grey/Grey.h"

AGPlayerCharacter::AGPlayerCharacter()
{

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>("Camera Boom");
	CameraBoom->SetupAttachment(GetRootComponent());
	// 相机旋转跟随pawn,拓展视角
	CameraBoom->bUsePawnControlRotation = true;

	CameraBoom->ProbeChannel = ECC_SpringArm;

	ViewCam = CreateDefaultSubobject<UCameraComponent>("View Cam");
	ViewCam->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);

	// 关闭角色跟随视角旋转，只通过输入控制
	bUseControllerRotationYaw = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 720.f, 0.f);
	
	
	HeroAttributeSet = CreateDefaultSubobject<UGHeroAttributeSet>("Hero Attribute Set");

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
		
		EnhancedInputComp->BindAction(LearnAbilityLeaderAction, ETriggerEvent::Started, this, &AGPlayerCharacter::LearnAbiltiyLeaderDown);
		EnhancedInputComp->BindAction(LearnAbilityLeaderAction, ETriggerEvent::Completed, this, &AGPlayerCharacter::LearnAbiltiyLeaderUp);

		
		for (const TPair<EGAbilityInputID, UInputAction*>& InputActionPair : GameplayAbilityInputActions)
		{
			// 这里给委托多荷载了一个inputID
			EnhancedInputComp->BindAction(InputActionPair.Value, ETriggerEvent::Triggered, this, &AGPlayerCharacter::HandleAbilityInput, InputActionPair.Key);
		}


	}
}

void AGPlayerCharacter::HandleLookInput(const FInputActionValue& InputActionValue)
{
	// 获取鼠标输入值（Vector2D: X=水平, Y=垂直）
	FVector2D InputVal = InputActionValue.Get<FVector2D>();

	// 视角移动
	AddControllerPitchInput(-InputVal.Y);
	AddControllerYawInput(InputVal.X);
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

void AGPlayerCharacter::HandleAbilityInput(const FInputActionValue& InputActionValue, EGAbilityInputID InputID)
{
	bool bPressed = InputActionValue.Get<bool>();
	
	if (bPressed && bIsLearnAbilityLeaderDown)
	{
		UpgradeAbilityWithInputID(InputID);
		return;
	}
	if (bPressed)
	{
		// 绑定触发输入到 inputID
		GetAbilitySystemComponent()->AbilityLocalInputPressed((int32)InputID);
	}
	else
	{
		GetAbilitySystemComponent()->AbilityLocalInputReleased((int32)InputID);
	}

	if (InputID == EGAbilityInputID::BasicAttack)
	{
		// 注意：引擎自带的bPressed输入流是有同步和预测的，自己写一套直接向ASC发送Pressed的事件,激活 GA 监听input task，默认是不会同步的
		// 注意： 左键按下这个输入事件只在客户端有，所以服务器是不会 send 这个事件的
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this , UGAbilitySystemStatics::GetBasicAttackInputPressedTag(), FGameplayEventData());

		Server_SendGameplayEventToSelf(UGAbilitySystemStatics::GetBasicAttackInputPressedTag(), FGameplayEventData());
	}
	
}

void AGPlayerCharacter::LearnAbiltiyLeaderDown(const FInputActionValue& InputActionValue)
{
	bIsLearnAbilityLeaderDown = true;
}

void AGPlayerCharacter::LearnAbiltiyLeaderUp(const FInputActionValue& InputActionValue)
{
	bIsLearnAbilityLeaderDown = false;
}

void AGPlayerCharacter::SetInputEnabledFromPlayerController(bool bEnabled)
{
	APlayerController* PlayerController = GetController<APlayerController>();
	if (!PlayerController)
	{
		return;
	}

	if (bEnabled)
	{
		EnableInput(PlayerController);
	}
	else
	{
		DisableInput(PlayerController);
	}
}

void AGPlayerCharacter::OnStun()
{
	SetInputEnabledFromPlayerController(false);
}

void AGPlayerCharacter::OnRecoverFromStun()
{
	SetInputEnabledFromPlayerController(true);
}

void AGPlayerCharacter::OnDead()
{
	SetInputEnabledFromPlayerController(false);
}

void AGPlayerCharacter::OnRespawn()
{
	SetInputEnabledFromPlayerController(true);
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

void AGPlayerCharacter::LerpCameraToLocalOffsetLocation(const FVector& Goal)
{
	GetWorldTimerManager().ClearTimer(CamerLerpTimerHandle);
	GetWorldTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &AGPlayerCharacter::TickCameraLocalOffsetLerp, Goal));
}

void AGPlayerCharacter::TickCameraLocalOffsetLerp(FVector Goal)
{
	FVector CurrentLocalOffset = ViewCam->GetRelativeLocation();
	if (FVector::Dist(CurrentLocalOffset, Goal) < 1.f)
	{
		ViewCam->SetRelativeLocation(Goal);
		return;
	}

	float LerpAlpha = FMath::Clamp(GetWorld()->GetDeltaSeconds() * CamerLerpSpeed, 0.f, 1.f);
	FVector NewLocalOffset = FMath::Lerp(CurrentLocalOffset, Goal, LerpAlpha);
	ViewCam->SetRelativeLocation(NewLocalOffset);

	GetWorldTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &AGPlayerCharacter::TickCameraLocalOffsetLerp, Goal));
}

void AGPlayerCharacter::OnAimStateChanged(bool bIsAimming)
{
	LerpCameraToLocalOffsetLocation(bIsAimming ? CameraAimLocalOffset : FVector{0.f});
}
