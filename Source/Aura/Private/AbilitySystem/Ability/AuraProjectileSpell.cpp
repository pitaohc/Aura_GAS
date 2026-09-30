// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Ability/AuraProjectileSpell.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Actor/AuraProjectile.h"
#include "Interaction/CombatInterface.h"

void UAuraProjectileSpell::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo*										ActorInfo,
	const FGameplayAbilityActivationInfo									ActivationInfo,
	const FGameplayEventData*												TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UAuraProjectileSpell::SpawnProjectile(const FVector& ProjectileTargetLocation)
{

	bool bIsServer = GetAvatarActorFromActorInfo()->HasAuthority();
	if (!bIsServer)
	{
		return;
	}

	if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetAvatarActorFromActorInfo()))
	{
		check(ProjectileClass);

		FVector	 SocketLocation = CombatInterface->GetCombatSocketLocation();
		FRotator Rotation = (ProjectileTargetLocation - SocketLocation).Rotation();
		Rotation.Pitch = 0;

		FTransform Transform;
		Transform.SetLocation(SocketLocation);
		Transform.SetRotation(Rotation.Quaternion());

		AAuraProjectile* NewProjectile = GetWorld()->SpawnActorDeferred<AAuraProjectile>(ProjectileClass,
			Transform,
			GetAvatarActorFromActorInfo(),
			Cast<APawn>(GetAvatarActorFromActorInfo()),
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

		if (UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(
				GetAvatarActorFromActorInfo()))
		{
			NewProjectile->SpecHandle = ASC->MakeOutgoingSpec(
				DamageEffectClass, GetAbilityLevel(), ASC->MakeEffectContext());
			UE_LOG(LogTemp, Log, TEXT("Projectile SpecHandle: %s"), *DamageEffectClass->GetName());
			GEngine->AddOnScreenDebugMessage(-1,
				5.f,
				FColor::Green,
				FString::Printf(TEXT("Projectile SpecHandle: %s"), *DamageEffectClass->GetName()));
		}

		NewProjectile->FinishSpawning(Transform);
	}
}