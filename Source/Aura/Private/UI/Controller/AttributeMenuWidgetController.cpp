// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/Controller/AttributeMenuWidgetController.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/AttributeInfo.h"

void UAttributeMenuWidgetController::BroadcastInitialValue()
{
	UAuraAttributeSet* AuraAttributeSet = CastChecked<UAuraAttributeSet>(AttributeSet);
	check(AttributeInfoDataAsset);

	for (const auto& pair : AuraAttributeSet->TagsToAttributes)
	{
		BroadcastAttributeInfo(pair.Key, pair.Value());
	}
}

void UAttributeMenuWidgetController::BindCallbacksToDependencies()
{
	UAuraAttributeSet* AuraAttributeSet = CastChecked<UAuraAttributeSet>(AttributeSet);
	check(AttributeInfoDataAsset);

	for (const auto& pair : AuraAttributeSet->TagsToAttributes)
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(pair.Value())
			.AddLambda([this, pair](const FOnAttributeChangeData& ChangeData) {
				BroadcastAttributeInfo(pair.Key, pair.Value());
			});
	}
}

void UAttributeMenuWidgetController::BroadcastAttributeInfo(
	const FGameplayTag& AttributeTag, const FGameplayAttribute& Attribute) const
{
	UAuraAttributeSet* AuraAttributeSet = CastChecked<UAuraAttributeSet>(AttributeSet);
	FAuraAttributeInfo Info = AttributeInfoDataAsset->FindAttributeInfoByTag(AttributeTag);
	Info.AttributeValue = Attribute.GetNumericValue(AuraAttributeSet);
	AttributeInfoDelegate.Broadcast(Info);
}