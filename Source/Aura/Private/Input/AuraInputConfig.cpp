// Fill out your copyright notice in the Description page of Project Settings.

#include "Input/AuraInputConfig.h"

const UInputAction* UAuraInputConfig::GetInputActionByTag(const FGameplayTag& InputTag, bool bLogNotFound) const
{
	for (const auto& InputAction : InputActions)
	{
		if (InputAction.InputTag == InputTag)
		{
			return InputAction.InputAction;
		}
	}
	if (bLogNotFound)
	{
		UE_LOG(LogTemp, Warning, TEXT("InputAction with tag %s not found"), *InputTag.ToString());
	}
	return nullptr;
}