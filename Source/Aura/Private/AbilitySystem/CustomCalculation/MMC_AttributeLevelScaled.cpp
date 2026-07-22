// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/CustomCalculation/MMC_AttributeLevelScaled.h"

UMMC_AttributeLevelScaled::UMMC_AttributeLevelScaled() {}

float UMMC_AttributeLevelScaled::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	FString AttributeName = AttributeDef.AttributeToCapture.AttributeName;
	FString IsSnapshot = AttributeDef.bSnapshot ? TEXT("Snapshot") : TEXT("Not Snapshot");
	FString CaptureSource = AttributeDef.AttributeSource == EGameplayEffectAttributeCaptureSource::Target
		? TEXT("Target")
		: TEXT("Source");
	FString info = FString::Format(
		TEXT("Attribute: {0}, Source: {1}, Snapshot: {2}"), { AttributeName, CaptureSource, IsSnapshot });
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, info);
	return Super::CalculateBaseMagnitude_Implementation(Spec);
}