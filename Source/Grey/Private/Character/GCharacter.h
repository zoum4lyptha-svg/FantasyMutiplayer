// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "GenericTeamAgentInterface.h"
#include "GameplayEffectTypes.h"
#include "GameplayTagContainer.h"
#include "GAS/GGameplayAbilityTypes.h"
#include "GCharacter.generated.h"

UCLASS()
class AGCharacter : public ACharacter, public IAbilitySystemInterface, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:

	AGCharacter();

	void ServerSideInit();
	void ClientSideInit();
	bool IsLocallyControlledByPlayer() const;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	const TMap<EGAbilityInputID, TSubclassOf<UGameplayAbility>>& GetAbilities() const;

	
protected:

	virtual void BeginPlay() override;
	// PossessedBy在当前 Controller被占用时触发(只在服务器触发)
	virtual void PossessedBy(AController* NewController) override;

public:	

	virtual void Tick(float DeltaTime) override;


	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


	/**********************************************************************/
	/*                             Gameplay Ability                       */
	/**********************************************************************/
public:
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_SendGameplayEventToSelf(const FGameplayTag& EventTag, const FGameplayEventData& EventData);

protected:
	void UpgradeAbilityWithInputID(EGAbilityInputID InputID);

private:


	// 这里实现角色组件监听所有GAS相关的tag的接口 
	void BindGASChangeDelegates();
	void DeathTagUpdated(const FGameplayTag Tag, int32 NewCount);

	void StunTagUpdated(const FGameplayTag Tag, int32 NewCount);
	
	void AimTagUpdated(const FGameplayTag Tag, int32 NewCount);
	void SetIsAimming(bool bIsAimming);
	
	virtual void OnAimStateChanged(bool bIsAimming);
	
	void MoveSpeedUpdated(const FOnAttributeChangeData& Data);
	
	// level up时从触发，补满生命
	void MaxHealthUpdated(const FOnAttributeChangeData& Data);
	void MaxManaUpdated(const FOnAttributeChangeData& Data);
	
	UPROPERTY(VisibleDefaultsOnly, Category = "Gameplay Ability")
	class UGAbilitySystemComponent* GAbilitySystemComponent;
	
	UPROPERTY()
	class UGAttributeSet* GAttributeSet;

	/**********************************************************************/
	/*                              UI                                    */
	/**********************************************************************/
private:
	UPROPERTY(VisibleDefaultsOnly, Category = "UI")
	class UWidgetComponent* OverHeadWidgetComponent;
	
	void ConfigureOverHeadStatusWidget();

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	float HeadStatGaugeVisiblityCheckUpdateGap = 1.f;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	float HeadStatGaugeVisiblityRangeSquared = 10000000.f;
	
	FTimerHandle HeadStatGaugeVisibilityUpdateTimerHandle;

	void UpdateHeadGaugeVisibility();

	void SetStatusGaugeEnabled(bool bIsEnabled);

	/**********************************************************************/
	/*                             Stun                                   */
	/**********************************************************************/
private:
	UPROPERTY(EditDefaultsOnly, Category = "Stun")
	UAnimMontage* StunMontage;

	virtual void OnStun();
	virtual void OnRecoverFromStun();
	/**********************************************************************/
	/*                             Death and Respawn                      */
	/**********************************************************************/

public:
	bool IsDead() const;
	void RespawnImmediately();
private:

	FTransform MeshRelativeTransform;
	UPROPERTY(EditDefaultsOnly, Category = "Death")
	float DeathMontageFinishTimeShift = -0.8f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Death")
	UAnimMontage* DeathMontage;

	FTimerHandle DeathMontageTimerHandle;

	void DeathMontageFinished();
	void SetRagdollEnabled(bool bIsEnabled);

	
	void PlayDeathAnimation();
	void StartDeathSequence();

	
	void Respawn();

	// 角色写个兜底的基类 死亡/重生
	virtual void OnDead();
	virtual void OnRespawn();


	/**********************************************************************/
	/*                               Team                                 */
	/**********************************************************************/
public:
	/** Assigns Team Agent to given TeamID */
	virtual void SetGenericTeamId(const FGenericTeamId& NewTeamID) override;
	
	/** Retrieve team identifier in form of FGenericTeamId */
	virtual FGenericTeamId GetGenericTeamId() const override;
private:
	UPROPERTY(ReplicatedUsing = OnRep_TeamID)
	FGenericTeamId TeamID;

	 
	UFUNCTION()
	virtual  void OnRep_TeamID();

	/**********************************************************************/
	/*                              被 AI 感知                               */
	/**********************************************************************/
private:
	void SetAIPerceptionStimuliSourceEnabled(bool bIsEnabled);
	
	UPROPERTY()
	class UAIPerceptionStimuliSourceComponent* PerceptionStimuliSourceComponent;
};
