// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "AuraPlayerController.generated.h"

class UAuraAbilitySystemComponent;
struct FGameplayTag;
class UInputMappingContext;
class UInputAction;
class IEnemyInterface;
struct FInputActionValue;
/**
 *
 */
UCLASS()
class AURA_API AAuraPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AAuraPlayerController();

	virtual void PlayerTick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override; // 不允许外部调用，但允许继承
	void		 Move(const FInputActionValue& InputActionValue);
	void		 CursorTrace();

	UPROPERTY(EditAnywhere, category = "Input")
	TObjectPtr<UInputMappingContext> AuraContext;

	UPROPERTY(EditAnywhere, category = "Input")
	TObjectPtr<UInputAction> MoveAction;

	// TODO TScriptInterface 的作用是什么？与TObjectPtr有什么区别？使用场景是什么？
	TScriptInterface<IEnemyInterface> LastActor;
	TScriptInterface<IEnemyInterface> ThisActor;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<class UAuraInputConfig> AuraInputConfig;

	void AbilityInputTagPressed(const FGameplayTag InputTag);
	void AbilityInputTagReleased(const FGameplayTag InputTag);
	void AbilityInputTagHeld(const FGameplayTag InputTag);

	UPROPERTY()
	TObjectPtr<UAuraAbilitySystemComponent> AuraAbilitySystemComponent;

	UAuraAbilitySystemComponent* GetASC();
	// Auto Run
	FVector CachedDestination = FVector::ZeroVector;
	float	FollowTime = 0.0f;
	float	ShortPressThreshold = 0.2f;
	bool	bAutoRunning = false;
	bool	bTargeting = false;

	UPROPERTY(EditDefaultsOnly)
	float AutoRunAcceptanceRadius = 50.0f;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class USplineComponent> Spline;
	void AutoRun();
	// Auto Run End
};