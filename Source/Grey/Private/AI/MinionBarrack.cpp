// Fill out your copyright notice in the Description page of Project Settings.


#include "MinionBarrack.h"

#include "Minion.h"
#include "GameFramework/PlayerStart.h"


// Sets default values
AMinionBarrack::AMinionBarrack()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AMinionBarrack::BeginPlay()
{
	Super::BeginPlay();
	// 只在服务器上生成对象，5s CD
	if (HasAuthority())
	{
		GetWorldTimerManager().SetTimer(SpawnIntervalTimerHandle, this, &AMinionBarrack::SpawnNewGroup, GroupSpawnInterval, true);
	}

	
}

// Called every frame
void AMinionBarrack::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

const APlayerStart* AMinionBarrack::GetNextSpawnSpot()
{
	if (SpawnSpots.Num() == 0)
	{
		return nullptr;
	}
	
	++NextSpawnSpotIndex;

	if (NextSpawnSpotIndex >= SpawnSpots.Num())
	{
		// 暂定一个出生点循环生成逻辑
		NextSpawnSpotIndex = 0;
	}

	return SpawnSpots[NextSpawnSpotIndex];
}

void AMinionBarrack::SpawnNewGroup()
{
	int i = MinionPerGroup;

	while (i > 0)
	{
		FTransform SpawnTransfrom = GetActorTransform();
		if (const APlayerStart* NextSpawnSpot = GetNextSpawnSpot())
		{
			SpawnTransfrom = NextSpawnSpot->GetActorTransform();
		}

		// 对象池中查找可复用对象（这里是“死亡”对象）
		AMinion* NextAvaliableMinion = GetNextAvaliableMinion();
		if (!NextAvaliableMinion)
			break;

		NextAvaliableMinion->SetActorTransform(SpawnTransfrom);
		NextAvaliableMinion->Activate();
		--i;
	}

	// 生成剩下的 Minions( i >= 0 )
	SpawnNewMinions(i);
}

void AMinionBarrack::SpawnNewMinions(int Amt)
{
	for (int i = 0; i < Amt; i++)
	{
		FTransform SpawnTransfrom = GetActorTransform();
		if (const APlayerStart* NextSpawnSpot = GetNextSpawnSpot())
		{
			SpawnTransfrom = NextSpawnSpot->GetActorTransform();
		}

		// 延迟生成（需要先指定team） --指定team -- 加入对象池备用
		// 注意： 延迟生成逻辑在生成小兵前就指定了teamID，所以 Minion,AIControl内部所有函数都可以直接拿到teamID
		AMinion* NewMinion = GetWorld()->SpawnActorDeferred<AMinion>(MinionClass, SpawnTransfrom, this, nullptr, ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);
		NewMinion->SetGenericTeamId(BarrackTeamId);
		NewMinion->FinishSpawning(SpawnTransfrom);
		NewMinion->SetGoal(MinionsGoal);
		MinionPool.Add(NewMinion);
	}
}

AMinion* AMinionBarrack::GetNextAvaliableMinion() const
{
	for(AMinion* Minion : MinionPool)
	{
		if (!Minion->IsActive())
		{
			// 复用死亡对象
			return Minion;
		}
	}

	return nullptr;
}

