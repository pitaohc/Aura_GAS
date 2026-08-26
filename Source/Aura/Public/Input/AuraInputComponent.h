// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AuraInputConfig.h"
#include "EnhancedInputComponent.h"
#include "AuraInputComponent.generated.h"

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class AURA_API UAuraInputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UAuraInputComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(
		float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	template <class UserClass, typename PressedFuncType, typename ReleasedFuncType, typename HeldFuncType>
	void BindAbilityActions(const class UAuraInputConfig* InputConfig,
		UserClass*										  UserObject,
		PressedFuncType									  PressedFunc,
		ReleasedFuncType								  ReleasedFunc,
		HeldFuncType									  HeldFunc);
};

template <class UserClass, typename PressedFuncType, typename ReleasedFuncType, typename HeldFuncType>
void UAuraInputComponent::BindAbilityActions(const UAuraInputConfig* InputConfig,
	UserClass*														 UserObject,
	PressedFuncType													 PressedFunc,
	ReleasedFuncType												 ReleasedFunc,
	HeldFuncType													 HeldFunc)
{
	check(InputConfig);

	for (const auto& InputAction : InputConfig->GetInputActions())
	{
		const bool IsValid = InputAction.InputAction && InputAction.InputTag.IsValid();
		if (!IsValid)
		{
			continue;
		}

		if (PressedFunc)
		{
			this->BindAction(
				InputAction.InputAction, ETriggerEvent::Started, UserObject, PressedFunc, InputAction.InputTag);
		}
		if (ReleasedFunc)
		{
			this->BindAction(
				InputAction.InputAction, ETriggerEvent::Completed, UserObject, ReleasedFunc, InputAction.InputTag);
		}
		if (HeldFunc)
		{
			this->BindAction(
				InputAction.InputAction, ETriggerEvent::Triggered, UserObject, HeldFunc, InputAction.InputTag);
		}
	}
}