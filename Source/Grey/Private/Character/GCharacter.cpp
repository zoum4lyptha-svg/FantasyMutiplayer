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
	GAbilitySystemComponent->InitAbilityActorInfo(this, this);
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

