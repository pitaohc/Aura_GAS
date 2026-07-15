// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/AuraCharacterBase.h"

#include "AbilitySystemComponent.h"

UAbilitySystemComponent* AAuraCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

// Sets default values
AAuraCharacterBase::AAuraCharacterBase()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need
	// it.
	PrimaryActorTick.bCanEverTick = false;

	weapon = CreateDefaultSubobject<USkeletalMeshComponent>("weapon"); // 相当于 make_ptr
	weapon->SetupAttachment(GetMesh(), FName("WeaponHandSocket"));
	weapon->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

// Called when the game starts or when spawned
void AAuraCharacterBase::BeginPlay()
{
	Super::BeginPlay();
}

void AAuraCharacterBase::InitAbilityActorInfo() {}

void AAuraCharacterBase::InitializePrimaryAttributes() const
{
	const auto ContextHandle = GetAbilitySystemComponent()->MakeEffectContext();
	const auto SpecHandle = GetAbilitySystemComponent()->MakeOutgoingSpec(
		DefaultPrimaryAttributeEffect, 1, ContextHandle);
	GetAbilitySystemComponent()->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), GetAbilitySystemComponent());
}