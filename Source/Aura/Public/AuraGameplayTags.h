// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class AURA_API FAuraGameplayTags
{
public:
	static FAuraGameplayTags& Get()
	{
		static FAuraGameplayTags instance;
		return instance;
	}
	static void                     InitializeNativeGameplayTags();
	// 禁止拷贝和移动
	FAuraGameplayTags(const FAuraGameplayTags&) = delete;
	FAuraGameplayTags& operator=(const FAuraGameplayTags&) = delete;
	FAuraGameplayTags(FAuraGameplayTags&&) = delete;
	FAuraGameplayTags& operator=(FAuraGameplayTags&&) = delete;
private:
	static FAuraGameplayTags GameplayTags;
	FAuraGameplayTags() = default;
	~FAuraGameplayTags() = default;
};