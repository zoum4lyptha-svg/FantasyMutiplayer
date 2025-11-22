// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class AGPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	// only called on the server
	void OnPossess(APawn* NewPawn) override;

	// only called on the client, also on the linstening server.
	void AcknowledgePossession(APawn* NewPawn) override;
private:
	UPROPERTY()
	class AGPlayerCharacter* GPlayerCharacter;
	
};
