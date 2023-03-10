// Fill out your copyright notice in the Description page of Project Settings.


#include "DebugSubsystem.h"

bool UDebugSubsystem::IsCategoryEnabled(const FName& CategoryName) const
{
	const bool* bIsEnabled = EnabledDebugCategoties.Find(CategoryName);
	return bIsEnabled != nullptr && *bIsEnabled;
}

void UDebugSubsystem::EnabledDebugCategory(const FName& CategoryName, bool bIsEnabled)
{
	EnabledDebugCategoties.FindOrAdd(CategoryName);
	EnabledDebugCategoties[CategoryName] = bIsEnabled;
}
