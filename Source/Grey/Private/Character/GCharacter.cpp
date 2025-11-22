// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/GCharacter.h"

#include "GAS/GAbilitySystemComponent.h"
#include "GAS/GAttributeSet.h"

#include "Components/SkeletalMeshComponent.h"


AGCharacter::AGCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	//把mesh的碰撞关了
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	GAbilitySystemComponent = CreateDefaultSubobject<UGAbilitySystemComponent>("GAbility System Component");
	GAttributeSet = CreateDefaultSubobject<UGAttributeSet>("GAttribute Set");
}

void AGCharacter::ServerSideInit()
{
	GAbilitySystemComponent->InitAbilityActorInfo(this, this);
	GAbilitySystemComponent->ApplyInitialEffects();
}

void AGCharacter::ClientSideInit()
{
	//客户端InitAbility的顺序需要在服务器确认后，否则很容易有问题
	GAbilitySystemComponent->InitAbilityActorInfo(this, this);
	// 客户端不会应用GE,客户端的GE来源于服务器的同步
	
}


void AGCharacter::BeginPlay()
{
	Super::BeginPlay();
	
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

