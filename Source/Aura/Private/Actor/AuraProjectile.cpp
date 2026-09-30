// Fill out your copyright notice in the Description page of Project Settings.

#include "Actor/AuraProjectile.h"

#include "NiagaraFunctionLibrary.h"
#include "Components/AudioComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

void AAuraProjectile::InitMovement()
{
	// Set Movement
	// ProjectileMovement->InitialSpeed = 100;
	// ProjectileMovement->MaxSpeed = 100;
	ProjectileMovement->ProjectileGravityScale = 0;
}

void AAuraProjectile::InitCollision()
{
	// Set Collision
	Sphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Sphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	Sphere->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
	Sphere->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Overlap);
	Sphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

// Sets default values
AAuraProjectile::AAuraProjectile()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	// Create Component
	Sphere = CreateDefaultSubobject<USphereComponent>("Sphere");
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovement");
	SetRootComponent(Sphere);

	InitCollision();
	InitMovement();
}

// Called when the game starts or when spawned
void AAuraProjectile::BeginPlay()
{
	Super::BeginPlay();
	Sphere->OnComponentBeginOverlap.AddDynamic(this, &AAuraProjectile::OnOverlap);

	if (HasAuthority())
	{
		SetLifeSpan(LifeSpan);
	}

	ValidateAssets();

	if (LoopSoundCue)
	{
		LoopSound = UGameplayStatics::SpawnSoundAttached(LoopSoundCue, GetRootComponent());
	}
}

void AAuraProjectile::ValidateAssets() const
{
	if (!LoopSoundCue)
	{
		UE_LOG(LogTemp,
			Warning,
			TEXT("%s (%s): LoopSoundCue is not set, projectile will have no looping sound."),
			*GetName(),
			*GetClass()->GetName());
	}
	if (!ImpactSoundCue)
	{
		UE_LOG(LogTemp,
			Warning,
			TEXT("%s (%s): ImpactSoundCue is not set, impact will have no sound."),
			*GetName(),
			*GetClass()->GetName());
	}
	if (!ImpactEffect)
	{
		UE_LOG(LogTemp,
			Warning,
			TEXT("%s (%s): ImpactEffect is not set, impact will have no Niagara effect."),
			*GetName(),
			*GetClass()->GetName());
	}
}

void AAuraProjectile::Destroyed()
{
	if (!bHit && !HasAuthority())
	{
		PlayImpactEffects();
	}
	if (LoopSound)
		LoopSound->Stop();
	Super::Destroyed();
}

void AAuraProjectile::OnOverlap(UPrimitiveComponent* OverlappedComponent,
	AActor*											 OtherActor,
	UPrimitiveComponent*							 OtherComp,
	int32											 OtherBodyIndex,
	bool											 bFromSweep,
	const FHitResult&								 SweepResult)
{
	if (!OtherActor || OtherActor == this || OtherActor == GetOwner())
		return;
	if (bHit)
	{
		return;
	}
	bHit = true;
	PlayImpactEffects();

	if (HasAuthority())
	{
		Destroy();
	}
}

void AAuraProjectile::PlayImpactEffects() const
{
	if (ImpactSoundCue)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSoundCue, GetActorLocation());
	}

	if (ImpactEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ImpactEffect, GetActorLocation());
	}

	if (LoopSound)
	{
		LoopSound->Stop();
	}
}