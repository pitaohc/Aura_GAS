// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/HUD/AuraHUD.h"
#include "Blueprint/UserWidget.h"

// 此处需要头文件，避免调用CreateWidget时找不到类型导致报错。
#include "UI/Controller/OverlayWidgetController.h"
#include "UI/Widget/AuraUserWidget.h"

UOverlayWidgetController* AAuraHUD::GetOverlayWidgetController(const FWidgetControllerParams& Params)
{
	if (OverlayWidgetController == nullptr)
	{
		OverlayWidgetController = NewObject<UOverlayWidgetController>(this, OverlayWidgetControllerClass);
		OverlayWidgetController->SetWidgetControllerParams(Params);
		OverlayWidgetController->BindCallbacksToDependencies();
	}
	return OverlayWidgetController;
}

void AAuraHUD::InitOverlay(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC, UAttributeSet* AS)
{
	checkf(OverlayWidgetClass, TEXT("Overlay Widget Class uninitialized, please fill out BP_AuraHUD"));
	checkf(OverlayWidgetControllerClass,
		TEXT("Overlay Widget Controller Class uninitialized, please fill out BP_AuraHUD"));

	UUserWidget* Widget = CreateWidget<UUserWidget>(GetWorld(), OverlayWidgetClass);
	OverlayWidget = Cast<UAuraUserWidget>(Widget);
	const FWidgetControllerParams params = FWidgetControllerParams(PC, PS, ASC, AS);
	UOverlayWidgetController*	  Controller = GetOverlayWidgetController(params);
	OverlayWidget->SetWidgetController(Controller);
	OverlayWidgetController->BroadcastInitialValue();
	Widget->AddToViewport();
}