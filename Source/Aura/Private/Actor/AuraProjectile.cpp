// Fill out your copyright notice in the Description page of Project Settings.

#include "Actor/AuraProjectile.h"

#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

void AAuraProjectile::InitMovement()
{
	// Set Movement
	ProjectileMovement->InitialSpeed = 100;
	ProjectileMovement->MaxSpeed = 100;
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
}

void AAuraProjectile::OnOverlap(UPrimitiveComponent* OverlappedComponent,
	AActor*											 OtherActor,
	UPrimitiveComponent*							 OtherComp,
	int32											 OtherBodyIndex,
	bool											 bFromSweep,
	const FHitResult&								 SweepResult)
{
	// TODO
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, TEXT("AAuraProjectile::OnOverlap"));
}