// Fill out your copyright notice in the Description page of Project Settings.


#include "FeameWork/GGameModeBase.h"

#include "EngineUtils.h"
#include "GameFramework/PlayerStart.h"

APlayerController* AGGameModeBase::SpawnPlayerController(ENetRole InRemoteRole, const FString& Options)
{
	APlayerController* NewPlayerController = Super::SpawnPlayerController(InRemoteRole, Options);
	IGenericTeamAgentInterface* NewPlayerTeamInterface = Cast<IGenericTeamAgentInterface>(NewPlayerController);
	FGenericTeamId TeamId = GetTeamIDForPlayer(NewPlayerController);
	if (NewPlayerTeamInterface)
	{
		NewPlayerTeamInterface->SetGenericTeamId(TeamId);
	}

	NewPlayerController->StartSpot = FindNextStartSpotForTeam(TeamId);
	return NewPlayerController;
}

// todo: 这里写死了一个简单的分组逻辑，后续匹配时要修改手动分组
FGenericTeamId AGGameModeBase::GetTeamIDForPlayer(const APlayerController* PlayerController) const
{
	static int PlayerCount = 0;
	++PlayerCount;
	return FGenericTeamId(PlayerCount % 2);
}

AActor* AGGameModeBase::FindNextStartSpotForTeam(const FGenericTeamId& TeamID) const
{
	const FName* StartSpotTag = TeamID2PlayerStartTagMap.Find(TeamID);
	if (!StartSpotTag)
	{
		return nullptr;
	}

	UWorld* World = GetWorld();
	
	for (TActorIterator<APlayerStart> It(World); It; ++It)
	{
		if (It->PlayerStartTag == *StartSpotTag)
		{
			// 标记为已占用,返回找到的player start
			It->PlayerStartTag = FName("Taken");
			return *It;
		}
	}

	return nullptr;
}


