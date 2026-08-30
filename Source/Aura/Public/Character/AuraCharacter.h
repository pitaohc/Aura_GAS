// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AuraCharacterBase.h"
#include "AuraCharacter.generated.h"

UCLASS()
class AURA_API AAuraCharacter : public AAuraCharacterBase
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AAuraCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	UPROPERTY(EditAnywhere, Category = "Character Movement")
	FRotator RotatorRateDefault = FRotator(0.0f, 100.0f, 0.0f);

protected:
	virtual void InitAbilityActorInfo() override;
	
	
public:
	/** Combat Interface */
	virtual int32   GetPlayerLevel() const override;
	UPROPERTY(EditAnywhere, Category = "Combat")
	FName CombatSocketName;
	virtual FVector GetCombatSocketLocation() const override;
	/** Combat Interface End */
};