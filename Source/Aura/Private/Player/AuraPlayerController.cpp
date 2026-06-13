// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/AuraPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Interaction/EnemyInterface.h"

AAuraPlayerController::AAuraPlayerController()
{
	bReplicates = true;
}

void AAuraPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	this->CursorTrace();
}

void AAuraPlayerController::BeginPlay()
{
	Super::BeginPlay();

	check(AuraContext);

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	check(Subsystem);

	Subsystem->AddMappingContext(AuraContext, 0);

	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;

	FInputModeGameAndUI InputModeData;
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputModeData.SetHideCursorDuringCapture(false);
	SetInputMode(InputModeData);

	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);
	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AAuraPlayerController::Move);

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
	FHitResult CursorResult;
	// TODO GetHitResultUnderCursor, GetHitResultUnderCursorByChannel, GetHitResultUnderCursorForObjects 对比
	// 三个参数：碰撞通道，是否复杂碰撞，结果
	GetHitResultUnderCursor(ECC_Visibility, false, CursorResult);
	if (!CursorResult.bBlockingHit)
		return;

	// 修改记录结果
	LastActor = ThisActor;
	ThisActor = CursorResult.GetActor();
	
	const bool HasLastActor = LastActor.GetInterface() != nullptr;
	const bool HasThisActor = ThisActor.GetInterface() != nullptr;
	const bool IsSameActor = ThisActor == LastActor;
	if (HasLastActor && HasThisActor && !IsSameActor) // 如果上一个和当前都有，并且不相同
	{
		LastActor.GetInterface()->UnHighLightActor();
		ThisActor.GetInterface()->HighLightActor();
	}
	else if (HasLastActor && !HasThisActor) // 如果上一个有，当前没有
	{
		LastActor.GetInterface()->UnHighLightActor();
	}
	else if (!HasLastActor && HasThisActor) // 如果上一个没有，当前有
	{
		ThisActor.GetInterface()->HighLightActor();
	}
}