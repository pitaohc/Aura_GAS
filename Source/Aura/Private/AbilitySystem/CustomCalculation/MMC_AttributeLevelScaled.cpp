// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/CustomCalculation/MMC_AttributeLevelScaled.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystemComponent.h"
#include "Interaction/CombatInterface.h"

UMMC_AttributeLevelScaled::UMMC_AttributeLevelScaled() {}

void UMMC_AttributeLevelScaled::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	RelevantAttributesToCapture.Empty();
	// UE_LOG(LogTemp, Warning, TEXT("UMMC_AttributeLevelScaled::PostEditChangeProperty AttributeDef.IsValid=%d,
	// AttrName=%s"), 	AttributeDef.AttributeToCapture.IsValid() ? 1 : 0,
	// 	*AttributeDef.AttributeToCapture.AttributeName);
	if (AttributeDef.AttributeToCapture.IsValid() && RelevantAttributesToCapture.Find(AttributeDef) == INDEX_NONE)
	{
		RelevantAttributesToCapture.Add(AttributeDef);
		UE_LOG(LogTemp,
			Warning,
			TEXT("UMMC_AttributeLevelScaled::PostEditChangeProperty Added to RelevantAttributesToCapture, Num()=%d"),
			RelevantAttributesToCapture.Num());
	}
}

float UMMC_AttributeLevelScaled::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	// FString AttributeName = AttributeDef.AttributeToCapture.AttributeName;
	// FString IsSnapshot = AttributeDef.bSnapshot ? TEXT("Snapshot") : TEXT("Not Snapshot");
	// FString CaptureSource = AttributeDef.AttributeSource == EGameplayEffectAttributeCaptureSource::Target
	// 	? TEXT("Target")
	// 	: TEXT("Source");
	// FString info = FString::Format(
	// 	TEXT("Attribute: {0}, Source: {1}, Snapshot: {2}"),
	// 	{ AttributeName, CaptureSource, IsSnapshot });
	// UE_LOG(LogTemp, Warning, TEXT("MMC: RelevantAttributesToCapture.Num()=%d, AttributeDef.IsValid=%d"),
	// RelevantAttributesToCapture.Num(), AttributeDef.AttributeToCapture.IsValid() ? 1 : 0);
	//
	// GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, info);
	// UE_LOG(LogTemp,Log, TEXT("UMMC_AttributeLevelScaled::CalculateBaseMagnitude_Implementation: %s"), *info);

	const FGameplayTagContainer* SourceContainer = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetContainer = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = SourceContainer;
	EvaluateParameters.TargetTags = TargetContainer;

	float AttributeMagnitude = 0.0f;
	bool  bCaptured = GetCapturedAttributeMagnitude(AttributeDef, Spec, EvaluateParameters, AttributeMagnitude);
	// UE_LOG(LogTemp, Warning, TEXT("MMC: bCaptured=%d, AttributeMagnitude=%f"), bCaptured, AttributeMagnitude);

	// UAbilitySystemComponent* SourceASC = Spec.GetContext().GetOriginalInstigatorAbilitySystemComponent();
	// AActor* TargetActor = Spec.GetContext().GetEffectCauser();
	// UE_LOG(LogTemp, Warning, TEXT("MMC: SourceASC=%p, TargetActor=%p"), SourceASC, TargetActor);
	//
	// if (SourceASC)
	// {
	// 	FGameplayAttribute StrengthAttr = UAuraAttributeSet::GetStrengthAttribute();
	// 	FGameplayAttribute VigorAttr = UAuraAttributeSet::GetVigorAttribute();
	// 	FGameplayAttribute IntelligenceAttr = UAuraAttributeSet::GetIntelligenceAttribute();
	// 	FGameplayAttribute ResilienceAttr = UAuraAttributeSet::GetResilienceAttribute();
	//
	// 	bool bHasStrength = SourceASC->HasAttributeSetForAttribute(StrengthAttr);
	// 	bool bHasVigor = SourceASC->HasAttributeSetForAttribute(VigorAttr);
	// 	bool bHasIntelligence = SourceASC->HasAttributeSetForAttribute(IntelligenceAttr);
	// 	bool bHasResilience = SourceASC->HasAttributeSetForAttribute(ResilienceAttr);
	//
	// 	float StrengthValue = bHasStrength ? SourceASC->GetNumericAttribute(StrengthAttr) : -1.0f;
	// 	float VigorValue = bHasVigor ? SourceASC->GetNumericAttribute(VigorAttr) : -1.0f;
	// 	float IntelligenceValue = bHasIntelligence ? SourceASC->GetNumericAttribute(IntelligenceAttr) : -1.0f;
	// 	float ResilienceValue = bHasResilience ? SourceASC->GetNumericAttribute(ResilienceAttr) : -1.0f;
	//
	// 	UE_LOG(LogTemp, Warning, TEXT("MMC: ASC Strength=%f, Vigor=%f, Intelligence=%f, ResilienceValue=%f"),
	// 		StrengthValue, VigorValue, IntelligenceValue, ResilienceValue);
	// }

	AttributeMagnitude = FMath::Max(AttributeMagnitude, 0.0f);

	int32 Level = 0;
	if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(Spec.GetContext().GetSourceObject()))
	{
		// UE_LOG(LogTemp,Log,TEXT("UMMC_AttributeLevelScaled::CalculateBaseMagnitude_Implementation: Capture
		// SourceObject"))
		Level = CombatInterface->GetPlayerLevel();
	}

	float result = BaseValue + Level * LevelScale + AttributeMagnitude * AttributeScale;
	UE_LOG(LogTemp,
		Log,
		TEXT(
			"UMMC_AttributeLevelScaled::CalculateBaseMagnitude_Implementation: BaseValue: %f, Level: %d, LevelScale: %f, AttributeMagnitude: %f, AttributeScale: %f, Result: %f"),
		BaseValue,
		Level,
		LevelScale,
		AttributeMagnitude,
		AttributeScale,
		result);
	return result;
}