// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "AuraInputConfig.generated.h"

class UInputAction;

USTRUCT(BlueprintType)
struct FAuraInputAction
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	const UInputAction* InputAction;
	UPROPERTY(EditDefaultsOnly)
	FGameplayTag InputTag;
};

/**
 *
 */
UCLASS()
class AURA_API UAuraInputConfig : public UDataAsset
{
	GENERATED_BODY()

private:
	UPROPERTY(EditDefaultsOnly)
	TArray<FAuraInputAction> InputActions;

public:
	const TArray<FAuraInputAction>& GetInputActions() const { return InputActions; }
	const UInputAction*				GetInputActionByTag(const FGameplayTag& InputTag, bool bLogNotFound = false) const;
};