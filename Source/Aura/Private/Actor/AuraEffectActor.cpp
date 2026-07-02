// Fill out your copyright notice in the Description page of Project Settings.

#include "Actor/AuraEffectActor.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemLog.h"

AAuraEffectActor::AAuraEffectActor()
{
	PrimaryActorTick.bCanEverTick = false;

	SetRootComponent(CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot")));
}

void AAuraEffectActor::BeginPlay()
{
	Super::BeginPlay();
}

void AAuraEffectActor::ApplyEffectToTarget(AActor* TargetActor, TSubclassOf<UGameplayEffect> GameplayEffectClass)
{
	if (!GameplayEffectClass)
	{
		UE_LOG(LogGameplayEffects, Warning, TEXT("GameplayEffectClass is invalid"));
		return;
	}
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	if (!TargetASC)
	{
		return;
	}
	check(GameplayEffectClass);
	FGameplayEffectContextHandle EffectContext = TargetASC->MakeEffectContext();
	EffectContext.AddSourceObject(this);
	const FGameplayEffectSpecHandle SpecHandle = TargetASC->MakeOutgoingSpec(GameplayEffectClass, 1, EffectContext);
	FActiveGameplayEffectHandle		ActiveHandle = TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);

	bool bIsInfinite = SpecHandle.Data->Def.Get()->DurationPolicy == EGameplayEffectDurationType::Infinite;

	if (bIsInfinite && InfiniteRemovePolicy == EEffectRemovePolicy::RemoveOnEndOverlap)
	{
		ActiveEffects.Add(ActiveHandle, TargetASC);
	}
}

void AAuraEffectActor::OnOverlap(AActor* TargetActor)
{
	if (InstantApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap)
	{
		ApplyEffectToTarget(TargetActor, InstantGameplayEffectClass);
	}
	if (DurationApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap)
	{
		ApplyEffectToTarget(TargetActor, DurationGameplayEffectClass);
	}
	if (InfiniteApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap)
	{
		ApplyEffectToTarget(TargetActor, InfiniteGameplayEffectClass);
	}
}

void AAuraEffectActor::OnEndOverlap(AActor* TargetActor)
{
	if (InstantApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap)
	{
		ApplyEffectToTarget(TargetActor, InstantGameplayEffectClass);
	}
	if (DurationApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap)
	{
		ApplyEffectToTarget(TargetActor, DurationGameplayEffectClass);
	}
	if (InfiniteApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap)
	{
		ApplyEffectToTarget(TargetActor, InfiniteGameplayEffectClass);
	}
	if (InfiniteRemovePolicy == EEffectRemovePolicy::RemoveOnEndOverlap)
	{
		UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
		if (!TargetASC)
		{
			return;
		}
		TArray<FActiveGameplayEffectHandle> ActiveHandlesToRemove;

		for (const auto& Pair : ActiveEffects)
		{
			if (TargetASC == Pair.Value)
			{
				TargetASC->RemoveActiveGameplayEffect(Pair.Key, 1);
				ActiveHandlesToRemove.Add(Pair.Key);
			}
		}
		for (const auto& Handle : ActiveHandlesToRemove)
		{
			ActiveEffects.FindAndRemoveChecked(Handle);
		}
	}
}