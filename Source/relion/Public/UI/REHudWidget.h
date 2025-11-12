// Copyright Alex Hajdu, 2025

#pragma once

#include "Blueprint/UserWidget.h"
#include "REHudWidget.generated.h"

UCLASS(Abstract)
class RELION_API UREHudWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent, Category="Widget")
	void OnDisplayMessage(const FText& Message);
};