// Copyright Alex Hajdu, 2025

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "REPlayerController.generated.h"

class UREHudWidget;

UCLASS()
class RELION_API AREPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AREPlayerController();

	void HandleHudUI(bool bShouldBeVisible, const FText& Message) const;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category="UI")
	TSubclassOf<UREHudWidget> WidgetClass;

private:
	UPROPERTY()
	TObjectPtr<UREHudWidget> CachedWidget;
};