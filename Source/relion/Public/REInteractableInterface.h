// Copyright Alex Hajdu, 2025

#pragma once

#include "REInteractableInterface.generated.h"

///@note I reused the same interface for both levers and objects that could be interactable by lever. 
///In a way both represents interactable objects from the programming POV even one looks only as a visual representation.
///I consider it fine for this time limited example

UINTERFACE(Blueprintable)
class UREInteractableInterface : public UInterface
{
	GENERATED_BODY()
};

class IREInteractableInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void InteractStart(AActor* Interactor);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void InteractEnd(AActor* Interactor);
	
	///@note Possibly add bool CanInteract() for more robustness
};