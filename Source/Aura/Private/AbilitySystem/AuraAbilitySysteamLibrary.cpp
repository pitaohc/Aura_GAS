// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/AuraAbilitySysteamLibrary.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "GameFramework/GameSession.h"
#include "Kismet/GameplayStatics.h"
#include "Player/AuraPlayerState.h"
#include "UI/Controller/AuraWidgetController.h"
#include "UI/HUD/AuraHUD.h"

UOverlayWidgetController* UAuraAbilitySysteamLibrary::GetOverlayWidgetController(const UObject* WorldContextObject)
{
	APlayerController* PC = UGameplayStatics::GetPlayerControllerFromID(WorldContextObject, 0);
	if (!PC)
		return nullptr;
	AAuraHUD* AuraHUD = Cast<AAuraHUD>(PC->GetHUD());
	if (!AuraHUD)
		return nullptr;
	AAuraPlayerState* PS = PC->GetPlayerState<AAuraPlayerState>();
	if (!PS)
		return nullptr;
	UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
	if (!ASC)
		return nullptr;
	UAttributeSet* AS = PS->GetAttributeSet();
	if (!AS)
		return nullptr;

	const FWidgetControllerParams Params = { PC, PS, ASC, AS };
	UOverlayWidgetController*	  OverlayController = AuraHUD->GetOverlayWidgetController(Params);
	return OverlayController;
}