// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "AuraProjectile.generated.h"

UCLASS()
class AURA_API AAuraProjectile : public AActor
{
	GENERATED_BODY()

public:
	void InitMovement();
	void InitCollision();
	// Sets default values for this actor's properties
	AAuraProjectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void Destroyed() override;
	UFUNCTION()
	void OnOverlap(UPrimitiveComponent* OverlappedComponent,
		AActor*							OtherActor,
		UPrimitiveComponent*			OtherComp,
		int32							OtherBodyIndex,
		bool							bFromSweep,
		const FHitResult&				SweepResult);

	bool bHit = false;

	void PlayImpactEffects() const;

	/** 检查美术/音效资源是否配置，缺失时输出 Warning（仅 BeginPlay 调用一次，避免刷屏） */
	void ValidateAssets() const;

public:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USphereComponent> Sphere;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UProjectileMovementComponent> ProjectileMovement;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UAudioComponent> LoopSound;
	UPROPERTY(EditDefaultsOnly)
	float LiveSpan = 30.0f;

	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundBase> LoopSoundCue;
	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundBase> ImpactSoundCue;
	UPROPERTY(EditAnywhere)
	TObjectPtr<class UNiagaraSystem> ImpactEffect;
};