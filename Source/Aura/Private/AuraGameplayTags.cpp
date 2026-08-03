// Fill out your copyright notice in the Description page of Project Settings.

#include "AuraGameplayTags.h"

#include "GameplayTagsManager.h"

void FAuraGameplayTags::InitializeNativeGameplayTags()
{
	UGameplayTagsManager& Manager = UGameplayTagsManager::Get();
	FAuraGameplayTags&	  Tags = FAuraGameplayTags::Get();
	// Primary
	Tags.Attributes_Primary_Strength = Manager.AddNativeGameplayTag(
		FName("Attributes.Primary.Strength"), FString("力量"));

	Tags.Attributes_Primary_Intelligence = Manager.AddNativeGameplayTag(
		FName("Attributes.Primary.Intelligence"), FString("智力"));
	Tags.Attributes_Primary_Resilience = Manager.AddNativeGameplayTag(
		FName("Attributes.Primary.Resilience"), FString("抗性"));
	Tags.Attributes_Primary_Vigor = Manager.AddNativeGameplayTag(FName("Attributes.Primary.Vigor"), FString("活力"));

	// Secondary
	Tags.Attributes_Secondary_Armor = Manager.AddNativeGameplayTag(
		FName("Attributes.Secondary.Armor"), FString("Reduces damage taken,improves Block Chance"));

	Tags.Attributes_Secondary_ArmorPenetration = Manager.AddNativeGameplayTag(
		FName("Attributes.Secondary.ArmorPenetration"),
		FString("Ignores Percentage of enemy Armor, increases Critical Hit Chance"));

	Tags.Attributes_Secondary_BlockChance = Manager.AddNativeGameplayTag(
		FName("Attributes.Secondary.BlockChance"), FString("Chance to cut incoming damage in half"));

	Tags.Attributes_Secondary_CriticalHitChance = Manager.AddNativeGameplayTag(
		FName("Attributes.Secondary.CriticalHitChance"), FString("暴击概率"));

	Tags.Attributes_Secondary_CriticalHitDamage = Manager.AddNativeGameplayTag(
		FName("Attributes.Secondary.CriticalHitDamage"), FString("暴击伤害"));

	Tags.Attributes_Secondary_CriticalHitResistance = Manager.AddNativeGameplayTag(
		FName("Attributes.Secondary.CriticalHitResistance"), FString("暴击抵抗"));

	Tags.Attributes_Secondary_HealthRegeneration = Manager.AddNativeGameplayTag(
		FName("Attributes.Secondary.HealthRegeneration"), FString("血量恢复"));

	Tags.Attributes_Secondary_ManaRegeneration = Manager.AddNativeGameplayTag(
		FName("Attributes.Secondary.ManaRegeneration"), FString("法力恢复"));

	Tags.Attributes_Secondary_MaxHealth = Manager.AddNativeGameplayTag(
		FName("Attributes.Secondary.MaxHealth"), FString("血量上限"));

	Tags.Attributes_Secondary_MaxMana = Manager.AddNativeGameplayTag(
		FName("Attributes.Secondary.MaxMana"), FString("法力上限"));
}