// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/AbilityTask/TargetDataUnderMouse.h"

UTargetDataUnderMouse* UTargetDataUnderMouse::CreateTargetDataUnderMouse(UGameplayAbility* OwnerAbility)
{
	UTargetDataUnderMouse* MyObj = NewAbilityTask<UTargetDataUnderMouse>(OwnerAbility);
	return MyObj;
}

void UTargetDataUnderMouse::Activate()
{
	Super::Activate();
	APlayerController* PC = Ability->GetCurrentActorInfo()->PlayerController.Get();
	FHitResult		   HitResult;
	PC->GetHitResultUnderCursor(ECC_Visibility, false, HitResult);
	ValidData.Broadcast(HitResult.ImpactPoint);
}