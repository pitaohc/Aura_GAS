// Fill out your copyright notice in the Description page of Project Settings.

#include "Player/AuraPlayerController.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AuraGameplayTags.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "GameplayTagContainer.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Components/SplineComponent.h"
#include "Input/AuraInputComponent.h"
#include "Interaction/EnemyInterface.h"

AAuraPlayerController::AAuraPlayerController()
{
	bReplicates = true;
	Spline = CreateDefaultSubobject<USplineComponent>("Spline");
}

void AAuraPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	this->CursorTrace();
	this->AutoRun();
}

void AAuraPlayerController::BeginPlay()
{
	Super::BeginPlay();

	check(AuraContext);

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(
		GetLocalPlayer());
	if (Subsystem)
	{
		Subsystem->AddMappingContext(AuraContext, 0);
	}

	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;

	FInputModeGameAndUI InputModeData;
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputModeData.SetHideCursorDuringCapture(false);
	SetInputMode(InputModeData);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AAuraPlayerController::Move);
		if (UAuraInputComponent* AuraInputComponent = Cast<UAuraInputComponent>(EnhancedInputComponent))
		{
			AuraInputComponent->BindAbilityActions(AuraInputConfig,
				this,
				&AAuraPlayerController::AbilityInputTagPressed,
				&AAuraPlayerController::AbilityInputTagReleased,
				&AAuraPlayerController::AbilityInputTagHeld);
		}
	}
}

void AAuraPlayerController::Move(const FInputActionValue& InputActionValue)
{
	const FVector2D MovementVector = InputActionValue.Get<FVector2D>();

	const FRotator Rotation = GetControlRotation();
	const FRotator YawRotation = FRotator(0.f, Rotation.Yaw, 0.f);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	if (APawn* ControlledPawn = GetPawn())
	{
		ControlledPawn->AddMovementInput(ForwardDirection, MovementVector.Y);
		ControlledPawn->AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AAuraPlayerController::CursorTrace()
{
	// 进行射线检测，获取鼠标位置下的Actor

	// TODO GetHitResultUnderCursor, GetHitResultUnderCursorByChannel, GetHitResultUnderCursorForObjects 对比
	// 三个参数：碰撞通道，是否复杂碰撞，结果
	GetHitResultUnderCursor(ECC_Visibility, false, CursorResult);
	if (!CursorResult.bBlockingHit)
	{
		return;
	}

	// 修改记录结果
	LastActor = ThisActor;
	ThisActor = CursorResult.GetActor();

	const bool HasLastActor = LastActor.GetInterface() != nullptr;
	const bool HasThisActor = ThisActor.GetInterface() != nullptr;
	const bool IsSameActor = ThisActor == LastActor;
	if (!IsSameActor)
	{
		if (HasLastActor)
		{
			LastActor.GetInterface()->UnHighLightActor();
		}
		if (HasThisActor)
		{
			ThisActor.GetInterface()->HighLightActor();
		}
	}
}

void AAuraPlayerController::AbilityInputTagPressed(const FGameplayTag InputTag)
{
	// GEngine->AddOnScreenDebugMessage(-1,
	// 	5.f,
	// 	FColor::Green,
	// 	FString::Printf(TEXT("AbilityInputTagPressed: %s"), *InputTag.ToString()));
	if (InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_LMB))
	{
		bTargeting = ThisActor ? true : false;
		bAutoRunning = false;
		FollowTime = 0;
	}
}

void AAuraPlayerController::AbilityInputTagReleased(const FGameplayTag InputTag)
{
	const bool bShouldAbilityHeld = bTargeting || !InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_LMB);
	if (bShouldAbilityHeld)
	{
		if (UAuraAbilitySystemComponent* ASC = GetASC())
		{
			ASC->AbilityInputTagReleased(InputTag);
		}
	}
	else
	{
		APawn* ControlledPawn = GetPawn();
		if (ControlledPawn && FollowTime <= ShortPressThreshold)
		{
			if (UNavigationPath* NavigationPath = UNavigationSystemV1::FindPathToLocationSynchronously(
					this, ControlledPawn->GetActorLocation(), CachedDestination))
			{
				Spline->ClearSplinePoints();

				for (const FVector& Point : NavigationPath->PathPoints)
				{
					Spline->AddSplinePoint(Point, ESplineCoordinateSpace::World);
					DrawDebugSphere(GetWorld(), Point, 8.0f, 8, FColor::Green, false, 5.0f);
				}
				CachedDestination = NavigationPath->PathPoints.Last();
				bAutoRunning = true;
			}
		}
		FollowTime = 0.0f;
		bTargeting = false;
	}
}

void AAuraPlayerController::AbilityInputTagHeld(const FGameplayTag InputTag)
{

	// GEngine->AddOnScreenDebugMessage(1,
	// 	0.0f,
	// 	FColor::Green,
	// 	FString::Printf(TEXT("AbilityInputTagHeld: %s"), *InputTag.ToString()));

	const bool bShouldAbilityHeld = bTargeting || !InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_LMB);
	if (bShouldAbilityHeld)
	{
		// 调用ASC
		if (UAuraAbilitySystemComponent* ASC = GetASC())
		{
			ASC->AbilityInputTagHeld(InputTag);
		}
	}
	else
	{
		// 移动
		FollowTime += GetWorld()->GetDeltaSeconds();

		if (CursorResult.bBlockingHit)
		{
			CachedDestination = CursorResult.ImpactPoint;
		}

		if (APawn* ControlledPawn = GetPawn())
		{
			const FVector MoveDirection = (CachedDestination - ControlledPawn->GetActorLocation()).GetSafeNormal();
			ControlledPawn->AddMovementInput(MoveDirection);
		}
	}
}

UAuraAbilitySystemComponent* AAuraPlayerController::GetASC()
{
	if (AuraAbilitySystemComponent == nullptr)
	{
		AuraAbilitySystemComponent = Cast<UAuraAbilitySystemComponent>(
			UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn<APawn>()));
	}
	return AuraAbilitySystemComponent;
}

void AAuraPlayerController::AutoRun()
{
	if (!bAutoRunning)
	{
		return;
	}
	if (APawn* ControlledPawn = GetPawn())
	{
		// 获得位置
		const FVector& Location = Spline->FindLocationClosestToWorldLocation(
			ControlledPawn->GetActorLocation(), ESplineCoordinateSpace::World);
		// 获得方向
		const FVector& Direction = Spline->FindDirectionClosestToWorldLocation(Location, ESplineCoordinateSpace::World);
		// 设置运动
		ControlledPawn->AddMovementInput(Direction);
		// 判断是否到达
		float DistanceToDestination = (CachedDestination - Location).Length();
		if (DistanceToDestination < AutoRunAcceptanceRadius)
		{
			bAutoRunning = false;
		}
	}
}