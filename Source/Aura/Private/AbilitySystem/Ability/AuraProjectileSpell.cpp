// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Ability/AuraProjectileSpell.h"

#include "Actor/AuraProjectile.h"
#include "Interaction/CombatInterface.h"

void UAuraProjectileSpell::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo*										ActorInfo,
	const FGameplayAbilityActivationInfo									ActivationInfo,
	const FGameplayEventData*												TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UAuraProjectileSpell::SpawnProjectile()
{

	bool bIsServer = GetAvatarActorFromActorInfo()->HasAuthority();
	if (!bIsServer)
	{
		return;
	}

	if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetAvatarActorFromActorInfo()))
	{
		check(ProjectileClass);

		FTransform Transform;
		Transform.SetLocation(CombatInterface->GetCombatSocketLocation());
		// TODO: Set the Projectile Rotation
		AAuraProjectile* NewProjectile = GetWorld()->SpawnActorDeferred<AAuraProjectile>(ProjectileClass,
			Transform,
			GetAvatarActorFromActorInfo(),
			Cast<APawn>(GetAvatarActorFromActorInfo()),
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

		// TODO: Give the Projectile a Gameplay Effect Spec for causing Damage.

		NewProjectile->FinishSpawning(Transform);
	}
}