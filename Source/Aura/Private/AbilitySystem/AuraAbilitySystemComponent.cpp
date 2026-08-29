// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/AuraAbilitySystemComponent.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraGameplayAbility.h"

void UAuraAbilitySystemComponent::AbilityActorInfoSet()
{
	OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &UAuraAbilitySystemComponent::ClientEffectApplied);
}

void UAuraAbilitySystemComponent::AddCharacterAbilities(const TArray<TSubclassOf<class UGameplayAbility>>& abilities)
{
	for (const TSubclassOf<UGameplayAbility>& AbilityClass : abilities)
	{
		FGameplayAbilitySpec AbilitySpec{ AbilityClass, 1 };
		// GiveAbilityAndActivateOnce(AbilitySpec);
		if (const UAuraGameplayAbility* AuraGameplayAbility = Cast<UAuraGameplayAbility>(AbilitySpec.Ability))
		{
			AbilitySpec.DynamicAbilityTags.AddTag(AuraGameplayAbility->StartupTag);
			GiveAbility(AbilitySpec);
		}
	}
}

void UAuraAbilitySystemComponent::AbilityInputTagHeld(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid())
	{
		return;
	}

	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (!AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			continue;
		}

		// 如果AbilitySpec的DynamicAbilityTags中包含InputTag，则尝试激活该能力
		AbilitySpecInputPressed(AbilitySpec);
		if (!AbilitySpec.IsActive())
		{
			// GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("TryActivate Ability %s"),
			// *AbilitySpec.Ability->GetName()));
			TryActivateAbility(AbilitySpec.Handle);
		}
	}
}

void UAuraAbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid())
	{
		return;
	}

	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (!AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			continue;
		}

		// 如果AbilitySpec的DynamicAbilityTags中包含InputTag，则尝试取消激活该能力
		AbilitySpecInputReleased(AbilitySpec);
		// GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("AbilitySpecInputReleased Ability
		// %s"), *AbilitySpec.Ability->GetName()));
	}
}

void UAuraAbilitySystemComponent::ClientEffectApplied_Implementation(
	UAbilitySystemComponent* Target, const FGameplayEffectSpec& SpecApplied, FActiveGameplayEffectHandle ActiveHandle)
{
	FGameplayTagContainer TagContainer;
	SpecApplied.GetAllAssetTags(TagContainer);
	EffectAssetTags.Broadcast(TagContainer);
}