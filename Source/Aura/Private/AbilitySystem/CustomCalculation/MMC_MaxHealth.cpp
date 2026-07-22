// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/CustomCalculation/MMC_MaxHealth.h"

UMMC_MaxHealth::UMMC_MaxHealth() {}

float UMMC_MaxHealth::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	return Super::CalculateBaseMagnitude_Implementation(Spec);
}