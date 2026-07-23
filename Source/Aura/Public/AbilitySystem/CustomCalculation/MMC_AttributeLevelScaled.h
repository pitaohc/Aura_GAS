// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "MMC_AttributeLevelScaled.generated.h"

/**
 *
 */
UCLASS()
class AURA_API UMMC_AttributeLevelScaled : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()

public:
	UMMC_AttributeLevelScaled();
	virtual void  PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;

private:
	UPROPERTY(EditAnywhere)
	float BaseValue = 0.0f;
	UPROPERTY(EditAnywhere)
	float LevelScale = 1.0f;
	UPROPERTY(EditAnywhere)
	float AttributeScale = 1.0f;
	UPROPERTY(EditAnywhere)
	FGameplayEffectAttributeCaptureDefinition AttributeDef;

	/** 打印 ASC 的完整诊断信息（属性列表、目标属性值等） */
	void DebugPrintASCInfo(const UAbilitySystemComponent* ASC, const FString& ASCLabel) const;

	/** 打印 Spec 中已注册和已捕获的属性信息 */
	void DebugPrintSpecCaptureInfo(const FGameplayEffectSpec& Spec) const;
};