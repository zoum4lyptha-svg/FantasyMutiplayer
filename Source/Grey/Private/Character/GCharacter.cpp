// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/GCharacter.h"

#include "Components/SkeletalMeshComponent.h"


AGCharacter::AGCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	//把mesh的碰撞关了
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

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

