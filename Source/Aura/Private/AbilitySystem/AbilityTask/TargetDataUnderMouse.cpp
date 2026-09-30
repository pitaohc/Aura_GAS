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
	if (IsLocallyControlled())
	{
		SendMouseCursorData();
	}
	else
	{
		const FGameplayAbilitySpecHandle SpecHandle = GetAbilitySpecHandle();
		const FPredictionKey			 ActivationKey = GetActivationPredictionKey();
		UAbilitySystemComponent*		 ASC = AbilitySystemComponent.Get();
		if (!ASC)
		{
			UE_LOG(LogTemp,
				Warning,
				TEXT("[%s] AbilitySystemComponent is null, mouse cursor target data not processed."),
				*GetNameSafe(this));
			return;
		}

		// 绑定回调函数，当服务器收到客户端发送的目标数据时，会触发这个回调函数
		ASC->AbilityTargetDataSetDelegate(SpecHandle, ActivationKey)
			.AddUObject(this, &UTargetDataUnderMouse::OnTargetDataReplicatedCallback);
		// 检查是否已经有目标数据被设置，如果有，则立即调用回调函数
		const bool bCalledDelegate = ASC->CallReplicatedTargetDataDelegatesIfSet(SpecHandle, ActivationKey);
		if (!bCalledDelegate)
		{
			// 如果没有目标数据被设置，则将任务标记为等待远程玩家数据
			SetWaitingOnRemotePlayerData();
		}
	}
}

void UTargetDataUnderMouse::SendMouseCursorData() const
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
		UE_LOG(LogTemp,
			Warning,
			TEXT("[%s] PlayerController is null, mouse cursor target data not sent."),
			*GetNameSafe(this));
		return;
	}
	// 获取鼠标命中结果
	FHitResult HitResult;
	PC->GetHitResultUnderCursor(ECC_Visibility, false, HitResult);
	FGameplayAbilityTargetData_SingleTargetHit* Data = new FGameplayAbilityTargetData_SingleTargetHit(HitResult);
	FGameplayAbilityTargetDataHandle			TargetDataHandle;
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

void UTargetDataUnderMouse::OnTargetDataReplicatedCallback(
	const FGameplayAbilityTargetDataHandle& DataHandle, FGameplayTag ActivationTag)
{
	// 消费掉缓存的目标数据，避免重复使用，需要知道Ability和Task。
	AbilitySystemComponent->ConsumeClientReplicatedTargetData(GetAbilitySpecHandle(), GetActivationPredictionKey());
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		ValidData.Broadcast(DataHandle);
	}
}