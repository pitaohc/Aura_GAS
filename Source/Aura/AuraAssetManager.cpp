// Fill out your copyright notice in the Description page of Project Settings.

#include "AuraAssetManager.h"

#include "AuraGameplayTags.h"

UAuraAssetManager& UAuraAssetManager::Get()
{
	check(GEngine);
	UAuraAssetManager* AuraAssetManager = Cast<UAuraAssetManager>(GEngine->AssetManager);
	return *AuraAssetManager;
}

void UAuraAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();
	// UE_LOG(LogTemp, Warning, TEXT("UAuraAssetManager::StartInitialLoading"));
	FAuraGameplayTags::InitializeNativeGameplayTags();
}