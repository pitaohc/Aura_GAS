// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Ability/AuraProjectileSpell.h"

void UAuraProjectileSpell::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	GEngine->AddOnScreenDebugMessage(-1,5,FColor::White,TEXT("UAuraProjectileSpell::ActivateAbility C++"));
}