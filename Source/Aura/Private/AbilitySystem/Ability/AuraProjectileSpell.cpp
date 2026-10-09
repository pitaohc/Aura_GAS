// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Ability/AuraProjectileSpell.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AuraGameplayTags.h"
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
	AActor* SourceActor = GetAvatarActorFromActorInfo();
	bool	bIsServer = SourceActor->HasAuthority();
	if (!bIsServer)
	{
		return;
	}

	if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(SourceActor))
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
			SourceActor,
			Cast<APawn>(SourceActor),
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

		if (UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(SourceActor))
		{
			if (DamageEffectClass)
			{
				auto EffectContext = ASC->MakeEffectContext();
				EffectContext.AddSourceObject(SourceActor);

				auto SpecHandle = ASC->MakeOutgoingSpec(DamageEffectClass, GetAbilityLevel(), EffectContext);

				FAuraGameplayTags& Tags = FAuraGameplayTags::Get();
				const float		   DamageValue = Damage.GetValueAtLevel(GetAbilityLevel());
				UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, Tags.Damage, DamageValue);
				NewProjectile->SpecHandle = SpecHandle;
				UE_LOG(LogTemp, Log, TEXT("Projectile SpecHandle: %s"), *DamageEffectClass->GetName());
				// GEngine->AddOnScreenDebugMessage(-1,
				// 	5.f,
				// 	FColor::Green,
				// 	FString::Printf(TEXT("Projectile SpecHandle: %s"), *DamageEffectClass->GetName()));
			}
		}

		NewProjectile->FinishSpawning(Transform);
	}
}