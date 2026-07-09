// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/AuraAbilitySystemComponent.h"

void UAuraAbilitySystemComponent::AbilityActorInfoSet()
{
	OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &UAuraAbilitySystemComponent::EffectApplied);
}

void UAuraAbilitySystemComponent::EffectApplied(UAbilitySystemComponent* Target,
	const FGameplayEffectSpec&											 SpecApplied,
	FActiveGameplayEffectHandle											 ActiveHandle) const
{
	GEngine->AddOnScreenDebugMessage(1, 8.0f, FColor::Green, FString("Effect Applied!"));
}