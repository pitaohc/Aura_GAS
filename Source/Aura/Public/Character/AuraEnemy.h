// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AuraCharacterBase.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "Interaction/EnemyInterface.h"
#include "UI/Controller/OverlayWidgetController.h" // TODO: Hack Please extract signatures to a common file
#include "AuraEnemy.generated.h"

UCLASS()
class AURA_API AAuraEnemy : public AAuraCharacterBase, public IEnemyInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AAuraEnemy();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character Class Defaults")
	int32 Level = 1;
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void InitAbilityActorInfo() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/** Enemy Interface */
	virtual void HighLightActor() override;

	virtual void UnHighLightActor() override;
	/** Enemy Interface End*/
	
	/** Combat Interface */
	FORCEINLINE virtual int32 GetPlayerLevel() const override;

	/** Combat Interface End*/
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<class UWidgetComponent> HealthBar;
	
	UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
	FOnAttributeChangedSignature OnHealthChanged;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
	FOnAttributeChangedSignature OnMaxHealthChanged;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	ECharacterClass CharacterClass = ECharacterClass::Invalid;

	virtual void InitializeDefaultAttributes() const override;
};
