// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/AuraAbilitySystemLibrary.h"

#include "AbilitySystem/AuraAttributeSet.h"
#include "Game/AuraGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Player/AuraPlayerState.h"
#include "UI/Controller/AuraWidgetController.h"
#include "UI/HUD/AuraHUD.h"

UOverlayWidgetController* UAuraAbilitySystemLibrary::GetOverlayWidgetController(const UObject* WorldContextObject)
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
	UOverlayWidgetController*     OverlayController = AuraHUD->GetOverlayWidgetController(Params);
	return OverlayController;
}

UAttributeMenuWidgetController* UAuraAbilitySystemLibrary::GetAttributeMenuWidgetController(
	const UObject* WorldContextObject)
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

	const FWidgetControllerParams   Params = { PC, PS, ASC, AS };
	UAttributeMenuWidgetController* AttributeMenuWidgetController = AuraHUD->GetAttributeMenuWidgetController(Params);
	return AttributeMenuWidgetController;
}

void UAuraAbilitySystemLibrary::InitializeDefaultAttributes(const UObject* WorldContextObject,
	const ECharacterClass                                                  CharacterClass,
	const float                                                            Level,
	UAbilitySystemComponent*                                               ASC)
{
	if (CharacterClass == ECharacterClass::Invalid)
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid character class"));
		return;
	}
	AAuraGameModeBase* GameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject));
	if (!GameMode)
	{
		UE_LOG(LogTemp, Warning, TEXT("GameMode is not AAuraGameModeBase"));
		return;
	}
	if (!GameMode->CharacterClassInfo)
	{
		UE_LOG(LogTemp, Warning, TEXT("CharacterClassInfo is NULL"));
		return;
	}
	// const UEnum* ClassEnum = StaticEnum<ECharacterClass>();
	// const FString ClassNameString = ClassEnum
	// 	? ClassEnum->GetNameStringByValue(static_cast<int64>(CharacterClass))
	// 	: TEXT("Unknown");
	//
	// UE_LOG(LogTemp, Warning, TEXT("InitializeDefaultAttributes class %s level %f"), *ClassNameString, Level);
	
	AActor* AvatorActor = ASC->GetAvatarActor();

	UCharacterClassInfo*       CharacterClassInfo = GameMode->CharacterClassInfo;
	FCharacterClassDefaultInfo CharacterClassDefaultInfo = CharacterClassInfo->GetClassDefaultInfo(CharacterClass);

	auto PrimaryAttributesContextHandle = ASC->MakeEffectContext();
	PrimaryAttributesContextHandle.AddSourceObject(AvatorActor);
	const auto PrimaryAttributesSpecHandle = ASC->MakeOutgoingSpec(CharacterClassDefaultInfo.PrimaryAttributes,
		Level,
		PrimaryAttributesContextHandle);
	ASC->ApplyGameplayEffectSpecToTarget(*PrimaryAttributesSpecHandle.Data.Get(), ASC);

	auto SecondaryAttributesContextHandle = ASC->MakeEffectContext();
	SecondaryAttributesContextHandle.AddSourceObject(AvatorActor);
	const auto SecondaryAttributesSpecHandle = ASC->MakeOutgoingSpec(CharacterClassInfo->SecondaryAttributes,
		Level,
		SecondaryAttributesContextHandle);
	ASC->ApplyGameplayEffectSpecToTarget(*SecondaryAttributesSpecHandle.Data.Get(), ASC);

	auto VitalAttributesContextHandle = ASC->MakeEffectContext();
	VitalAttributesContextHandle.AddSourceObject(AvatorActor);
	const auto VitalAttributesSpecHandle = ASC->MakeOutgoingSpec(CharacterClassInfo->VitalAttributes,
		Level,
		VitalAttributesContextHandle);
	ASC->ApplyGameplayEffectSpecToTarget(*VitalAttributesSpecHandle.Data.Get(), ASC);
}