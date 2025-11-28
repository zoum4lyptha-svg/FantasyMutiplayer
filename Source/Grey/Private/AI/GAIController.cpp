// Fill out your copyright notice in the Description page of Project Settings.


#include "GAIController.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "BrainComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GAS/GAbilitySystemStatics.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"


AGAIController::AGAIController()
{
	AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>("AI Perception Component");
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>("Sight Config");

	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = false;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = false;

	SightConfig->SightRadius = 1000.f;
	SightConfig->LoseSightRadius = 1200.f;
	
	// 5秒后失去目标
	SightConfig->SetMaxAge(5.f);

	SightConfig->PeripheralVisionAngleDegrees = 180.f;

	AIPerceptionComponent->ConfigureSense(*SightConfig);
	// 监听感知变化事件，触发后向 blackboard 添加发现的 actor 作为 "Target"
	AIPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &AGAIController::TargetPerceptionUpdated);

	// 监听遗忘事件，触发后随机选取范围内新对象作为目标
	AIPerceptionComponent->OnTargetPerceptionForgotten.AddDynamic(this, &AGAIController::TargetForgotten);
}

void AGAIController::OnPossess(APawn* NewPawn)
{
	Super::OnPossess(NewPawn);
	

	IGenericTeamAgentInterface* PawnTeamInterface = Cast<IGenericTeamAgentInterface>(NewPawn);
	if (PawnTeamInterface)
	{
		SetGenericTeamId(PawnTeamInterface->GetGenericTeamId());
		ClearAndDisableAllSenses();
		EnableAllSenses();
	}

	UAbilitySystemComponent* PawnASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(NewPawn);
	if (PawnASC)
	{
		// AI control 也来监听死亡事件，死亡清理数据关闭AI,重生打开AI
		PawnASC->RegisterGameplayTagEvent(UGAbilitySystemStatics::GetDeadStatTag()).AddUObject(this, &AGAIController::PawnDeadTagUpdated);
		// AI control 监听晕眩 tag
		PawnASC->RegisterGameplayTagEvent(UGAbilitySystemStatics::GetStunStatTag()).AddUObject(this, &AGAIController::PawnStunTagUpdated);
	}
}

void AGAIController::BeginPlay()
{
	Super::BeginPlay();
	RunBehaviorTree(BehaviorTree);
}

void AGAIController::TargetPerceptionUpdated(AActor* TargetActor, FAIStimulus Stimulus)
{
	if (Stimulus.WasSuccessfullySensed())
	{
		// todo:这里只做了保持追踪同一目标的逻辑，后续可以考虑加个仇恨机制
		if (!GetCurrentTarget())
		{
			SetCurrentTarget(TargetActor);
		}
	}
	else
	{
		// AI锁定目标死亡时立即遗忘对象，切换目标
		ForgetActorIfDead(TargetActor);
	}
}

void AGAIController::TargetForgotten(AActor* ForgottenActor)
{
	if (!ForgottenActor)
		return;

	if (GetCurrentTarget() == ForgottenActor)
	{
		SetCurrentTarget(GetNextPerceivedActor());
	}
}

const UObject* AGAIController::GetCurrentTarget() const
{
	const UBlackboardComponent* BlackboardComponent = GetBlackboardComponent();
	if (BlackboardComponent)
	{
		return GetBlackboardComponent()->GetValueAsObject(TargetBlackboardKeyName);
	}
	return nullptr;
}

void AGAIController::SetCurrentTarget(AActor* NewTarget)
{
	UBlackboardComponent* BlackboardComponent = GetBlackboardComponent();
	if (!BlackboardComponent)
		return;

	if (NewTarget)
	{
		BlackboardComponent->SetValueAsObject(TargetBlackboardKeyName, NewTarget);
	}
	else
	{
		BlackboardComponent->ClearValue(TargetBlackboardKeyName);
	}
}

AActor* AGAIController::GetNextPerceivedActor() const
{
	if (PerceptionComponent)
	{
		TArray<AActor*> Actors;
		AIPerceptionComponent->GetPerceivedHostileActors(Actors);

		if (Actors.Num() != 0)
		{
			// todo: AI切换索敌的逻辑可以完善一下
			return Actors[0];
		}
	}

	return nullptr;
}

void AGAIController::ForgetActorIfDead(AActor* ActorToForget)
{
	// 如果丢失目标是因为目标actor已死亡，则立刻遗忘
	const UAbilitySystemComponent* ActorASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(ActorToForget);
	if (!ActorASC)
		return;

	if (ActorASC->HasMatchingGameplayTag(UGAbilitySystemStatics::GetDeadStatTag()))
	{
		for (UAIPerceptionComponent::TActorPerceptionContainer::TIterator Iter = AIPerceptionComponent->GetPerceptualDataIterator(); Iter; ++Iter)
		{
			if (Iter->Key != ActorToForget)
			{
				continue;
			}

			for (FAIStimulus& Stimuli : Iter->Value.LastSensedStimuli)
			{
				Stimuli.SetStimulusAge(TNumericLimits<float>::Max());
			}
		}
	}
}

void AGAIController::ClearAndDisableAllSenses()
{
	// 刺激过期 -- 关闭所有感知 -- 清理所有blackboard数据 之后重新开始
	AIPerceptionComponent->AgeStimuli(TNumericLimits<float>::Max());

	for (auto SenseConfigIt = AIPerceptionComponent->GetSensesConfigIterator(); SenseConfigIt; ++SenseConfigIt)
	{
		AIPerceptionComponent->SetSenseEnabled((*SenseConfigIt)->GetSenseImplementation(), false);
	}

	if (GetBlackboardComponent())
	{
		GetBlackboardComponent()->ClearValue(TargetBlackboardKeyName);
	}
}

void AGAIController::EnableAllSenses()
{
	for (auto SenseConfigIt = AIPerceptionComponent->GetSensesConfigIterator(); SenseConfigIt; ++SenseConfigIt)
	{
		AIPerceptionComponent->SetSenseEnabled((*SenseConfigIt)->GetSenseImplementation(), true);
	}
}

void AGAIController::PawnDeadTagUpdated(const FGameplayTag Tag, int32 Count)
{
	if (Count != 0)
	{
		GetBrainComponent()->StopLogic("Dead");
		ClearAndDisableAllSenses();
		bIsPawnDead = true;
	}
	else
	{
		GetBrainComponent()->StartLogic();
		EnableAllSenses();
		bIsPawnDead = false;
	}
}

void AGAIController::PawnStunTagUpdated(const FGameplayTag Tag, int32 Count)
{
	if (bIsPawnDead)
		return;

	if (Count != 0)
	{
		GetBrainComponent()->StopLogic("Stun");
	}
	else
	{
		GetBrainComponent()->StartLogic();
	}
}



