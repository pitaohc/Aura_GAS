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

		for (const auto& pair : AuraAttributeSet->TagsToAttributes)
	{
		const FGameplayTag&		  AttributeTag = pair.Key;
		const FGameplayAttribute& Attribute = pair.Value();
		FAuraAttributeInfo		  Info = AttributeInfoDataAsset->FindAttributeInfoByTag(AttributeTag);
		Info.AttributeValue = Attribute.GetNumericValue(AuraAttributeSet);
		AttributeInfoDelegate.Broadcast(Info);
	}
}

void UAttributeMenuWidgetController::BindCallbacksToDependencies()
{
	Super::BindCallbacksToDependencies();
}