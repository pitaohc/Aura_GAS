// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/Controller/OverlayWidgetController.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"

void UOverlayWidgetController::BroadcastInitialValue()
{
	const UAuraAttributeSet* AuraAttributeSet = CastChecked<UAuraAttributeSet>(AttributeSet);

	OnHealthChanged.Broadcast(AuraAttributeSet->GetHealth());
	OnMaxHealthChanged.Broadcast(AuraAttributeSet->GetMaxHealth());
	OnManaChanged.Broadcast(AuraAttributeSet->GetMana());
	OnMaxManaChanged.Broadcast(AuraAttributeSet->GetMaxMana());
}

void UOverlayWidgetController::BindCallbacksToDependencies()
{
	Super::BindCallbacksToDependencies();

	UAuraAttributeSet* AuraAttributeSet = CastChecked<UAuraAttributeSet>(AttributeSet);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetHealthAttribute())
	                      .AddLambda([this](const FOnAttributeChangeData& Data) {
		                      OnHealthChanged.Broadcast(Data.NewValue);
	                      });
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetMaxHealthAttribute())
	                      .AddLambda([this](const FOnAttributeChangeData& Data) {
		                      OnMaxHealthChanged.Broadcast(Data.NewValue);
	                      });
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetManaAttribute())
	                      .AddLambda([this](const FOnAttributeChangeData& Data) {
		                      OnManaChanged.Broadcast(Data.NewValue);
	                      });
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AuraAttributeSet->GetMaxManaAttribute())
	                      .AddLambda([this](const FOnAttributeChangeData& Data) {
		                      OnMaxManaChanged.Broadcast(Data.NewValue);
	                      });

	if (auto* AuraASC = Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent))
	{
		FGameplayTag MessageTag = FGameplayTag::RequestGameplayTag(TEXT("Message"));
		check(MessageWidgetDataTable);
		AuraASC->EffectAssetTags.AddLambda([this,MessageTag](const FGameplayTagContainer& TagContainer) {
			for (const FGameplayTag& Tag : TagContainer)
			{
				if (!Tag.MatchesTag(MessageTag))
				{
					continue;
				}
				if (FUIWidgetRow* row = GetDataTableRowByTag<FUIWidgetRow>(MessageWidgetDataTable, Tag))
				{
					const FString Msg = FString::Printf(TEXT("Widget GE Tag: %s"), *row->Message.ToString());
					GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, Msg);
					MessageWidgetRowDelegate.Broadcast(*row);
				}
			}
		});
	}
}
