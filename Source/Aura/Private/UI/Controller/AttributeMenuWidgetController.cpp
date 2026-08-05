// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/Controller/AttributeMenuWidgetController.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/AttributeInfo.h"

void UAttributeMenuWidgetController::BroadcastInitialValue()
{
	Super::BroadcastInitialValue();

	UAuraAttributeSet* AuraAttributeSet = CastChecked<UAuraAttributeSet>(AttributeSet);
	check(AttributeInfoDataAsset)

		FAuraAttributeInfo StrengthInfo = AttributeInfoDataAsset->FindAttributeInfoByTag(
			FAuraGameplayTags::Get().Attributes_Primary_Strength);
	StrengthInfo.AttributeValue = AuraAttributeSet->GetStrength();
	AttributeInfoDelegate.Broadcast(StrengthInfo);
}

void UAttributeMenuWidgetController::BindCallbacksToDependencies()
{
	Super::BindCallbacksToDependencies();
}