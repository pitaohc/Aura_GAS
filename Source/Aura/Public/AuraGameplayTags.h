// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NativeGameplayTags.h"

/**
 *
 */
class FAuraGameplayTags
{
public:
	static FAuraGameplayTags& Get()
	{
		static FAuraGameplayTags instance;
		return instance;
	}

	static void InitializeNativeGameplayTags();
	// 禁止拷贝和移动
	FAuraGameplayTags(const FAuraGameplayTags&) = delete;
	FAuraGameplayTags& operator=(const FAuraGameplayTags&) = delete;
	FAuraGameplayTags(FAuraGameplayTags&&) = delete;
	FAuraGameplayTags& operator=(FAuraGameplayTags&&) = delete;
	// Primary Attributes
	FGameplayTag Attributes_Primary_Strength;
	FGameplayTag Attributes_Primary_Intelligence;
	FGameplayTag Attributes_Primary_Resilience;
	FGameplayTag Attributes_Primary_Vigor;
	// Secondary Attributes
	FGameplayTag Attributes_Secondary_Armor;
	FGameplayTag Attributes_Secondary_ArmorPenetration;
	FGameplayTag Attributes_Secondary_BlockChance;
	FGameplayTag Attributes_Secondary_CriticalHitChance;
	FGameplayTag Attributes_Secondary_CriticalHitDamage;
	FGameplayTag Attributes_Secondary_CriticalHitResistance;
	FGameplayTag Attributes_Secondary_HealthRegeneration;
	FGameplayTag Attributes_Secondary_ManaRegeneration;
	FGameplayTag Attributes_Secondary_MaxHealth;
	FGameplayTag Attributes_Secondary_MaxMana;
	// Input Tags
	FGameplayTag InputTag_LMB;
	FGameplayTag InputTag_RMB;
	FGameplayTag InputTag_1;
	FGameplayTag InputTag_2;
	FGameplayTag InputTag_3;
	FGameplayTag InputTag_4;

private:
	FAuraGameplayTags() = default;
	~FAuraGameplayTags() = default;
};