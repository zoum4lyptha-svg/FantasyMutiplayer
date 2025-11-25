// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GenericTeamAgentInterface.h"
#include "GGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class AGGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	virtual APlayerController* SpawnPlayerController(ENetRole InRemoteRole, const FString& Options) override;

private:
	FGenericTeamId GetTeamIDForPlayer(const APlayerController* PlayerController) const;

	AActor* FindNextStartSpotForTeam(const FGenericTeamId& TeamID) const;

	// map < teamID ,蓝图 player start 配的 tag>映射
	UPROPERTY(EditDefaultsOnly, Category = "Team")
	TMap<FGenericTeamId, FName> TeamID2PlayerStartTagMap;
	
};
