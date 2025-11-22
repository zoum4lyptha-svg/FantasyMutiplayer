// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/GPlayerController.h"


#include "Player/GPlayerCharacter.h"

void AGPlayerController::OnPossess(APawn* NewPawn)
{
	Super::OnPossess(NewPawn);
	GPlayerCharacter = Cast<AGPlayerCharacter>(NewPawn);
	if (GPlayerCharacter)
	{
		GPlayerCharacter->ServerSideInit();
	}
}

void AGPlayerController::AcknowledgePossession(APawn* NewPawn)
{
	Super::AcknowledgePossession(NewPawn);
	GPlayerCharacter = Cast<AGPlayerCharacter>(NewPawn);
	if (GPlayerCharacter)
	{
		GPlayerCharacter->ClientSideInit();
	}
}