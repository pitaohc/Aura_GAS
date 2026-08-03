// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "AttributeInfo.generated.h"

USTRUCT(BlueprintType) // 由于添加了BlueprintReadOnly，所以需要添加BlueprintType，否则会报错
struct FAuraAttributeInfo
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag AttributeTag;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText AttributeName;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText AttributeDescription;
	UPROPERTY(BlueprintReadOnly) // 具体的值动态读取，不需要编辑时修改
	float AttributeValue;
};

/**
 *
 */
UCLASS()
class AURA_API UAttributeInfo : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FAuraAttributeInfo> AttributeInformation;

	FAuraAttributeInfo FindAttributeInfoByTag(const FGameplayTag& Tag, bool bLogNotFound = false) const;
};