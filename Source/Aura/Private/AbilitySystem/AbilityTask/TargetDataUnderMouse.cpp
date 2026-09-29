// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/AbilityTask/TargetDataUnderMouse.h"

#include "AbilitySystemComponent.h"

UTargetDataUnderMouse* UTargetDataUnderMouse::CreateTargetDataUnderMouse(UGameplayAbility* OwnerAbility)
{
	UTargetDataUnderMouse* MyObj = NewAbilityTask<UTargetDataUnderMouse>(OwnerAbility);
	return MyObj;
}

void UTargetDataUnderMouse::Activate()
{
	const bool bIsLocallyControlled = Ability->GetCurrentActorInfo()->IsLocallyControlled();
	if (bIsLocallyControlled)
	{
		SendCursorTargetData();
	}
	else
	{
		// TODO: We are on the server, so listen for target data.
	}
}

void UTargetDataUnderMouse::SendCursorTargetData() const
{
	// 创建一个作用域内的预测窗口，通过RAII的机制，为当前函数提供预测上下文。
	// 在此作用域内，ASC::ScopedPredictionKey 会获得这个PredictionKey。
	// PredictionKey是用于协调TargetData和Task Activate的时序问题的。
	// 这样服务器收到目标数据时，就能知道它对应客户端的哪一次预测，从而正确做预测确认、回滚或复制。
	FScopedPredictionWindow ScopedPrediction{ AbilitySystemComponent.Get() };
	// Task存储了当前Actor的信息，可以从GetCurrentActorInfo获取
	// GetHitResultUnderCursor方法在PlayerController上，我们需要通过Actor获得PlayerController
	// 由于PlayerController在Actor是一个WeakPtr，因此需要进行检查保护
	APlayerController* PC = Ability->GetCurrentActorInfo()->PlayerController.Get();
	if (!PC)
	{
		UE_LOG(LogTemp, Error, TEXT("PC is null"));
		return;
	}
	// 获取鼠标命中结果
	FHitResult         HitResult;
	PC->GetHitResultUnderCursor(ECC_Visibility, false, HitResult);
	FGameplayAbilityTargetData_SingleTargetHit* Data = new FGameplayAbilityTargetData_SingleTargetHit(HitResult);
	FGameplayAbilityTargetDataHandle            TargetDataHandle;
	TargetDataHandle.Add(Data);
	// 传递TargetData，各个参数的作用：
	// 1）标识Ability实例；
	// 2）和5）用于关联Ability和当前预测窗口，
	// 3）TargetDataHandle用于传递我们需要的数据，
	// 4）区分目标数据的类型或上下文，与具体业务相关
	AbilitySystemComponent->ServerSetReplicatedTargetData(GetAbilitySpecHandle(),
		GetActivationPredictionKey(),
		TargetDataHandle,
		FGameplayTag(),
		AbilitySystemComponent->ScopedPredictionKey);
	
	// TODO: 日后可以考虑改为UAbilityTask::IsActive()
	if (ShouldBroadcastAbilityTaskDelegates()) // 广播的条件Ability存在且被激活
	{
		// 广播获得的目标数据，可以用于在C/S绘制Debug位置
		ValidData.Broadcast(TargetDataHandle);
	}

}