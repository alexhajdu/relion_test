// Copyright Alex Hajdu, 2025

#include "Core/REPlayerController.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "REInteractableInterface.h"
#include "RELeverActor.h"
#include "Blueprint/UserWidget.h"
#include "UI/REHudWidget.h"

AREPlayerController::AREPlayerController()
{
	bShowMouseCursor = false;
	bEnableClickEvents = false;
	bEnableTouchEvents = false;
}

void AREPlayerController::HandleHudUI(const bool bShouldBeVisible, const FText& Message) const
{
	if (CachedWidget)
	{
		if (bShouldBeVisible)
		{
			CachedWidget->OnDisplayMessage(Message);
			CachedWidget->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}
		else
		{
			CachedWidget->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}

void AREPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// Create hud widget and add widget to viewport
	if (IsLocalController() && WidgetClass)
	{
		CachedWidget = CreateWidget<UREHudWidget>(this, WidgetClass);
		if (CachedWidget)
		{
			CachedWidget->AddToViewport();

			// Hide at start
			CachedWidget->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}
