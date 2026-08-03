// Fill out your copyright notice in the Description page of Project Settings.

#include "AbilitySystem/Data/AttributeInfo.h"

FAuraAttributeInfo UAttributeInfo::FindAttributeInfoByTag(const FGameplayTag& Tag, bool bLogNotFound) const
{
	for (const auto& elem : AttributeInformation)
	{
		if (elem.AttributeTag == Tag)
		{
			return elem;
		}
	}
	if (bLogNotFound)
	{
		UE_LOG(LogTemp,
			Warning,
			TEXT("Can not find Info for Attribute Tag [%s] on AttributeInfo DataAsset [%s]"),
			*Tag.ToString(),
			*GetName());
	}
	return FAuraAttributeInfo();
}