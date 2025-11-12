// Copyright Alex Hajdu, 2025

#pragma once

#include "GameFramework/Actor.h"
#include "REInteractableInterface.h"
#include "RELightActor.generated.h"

// Fwd. decl.
class UPointLightComponent;
class USoundBase;

UCLASS()
class RELION_API ARELightActor : public AActor, public IREInteractableInterface
{
    GENERATED_BODY()

public:
    ARELightActor();

protected:
    virtual void BeginPlay() override;

    // Light component
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    TObjectPtr<UPointLightComponent> PointLight;

    // Whether light is currently on
    UPROPERTY(ReplicatedUsing = OnRep_LightState, BlueprintReadOnly, Category="Light")
    bool bIsOn = true;

    // Optional color for light when ON/OFF
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Light|Visual")
    FLinearColor LightOnColor = FLinearColor::Red;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Light|Visual")
    FLinearColor LightOffColor = FLinearColor(0.05f, 0.05f, 0.05f);

public:
    // Interface Implementation
    virtual void InteractStart_Implementation(AActor* Interactor) override;
    virtual void InteractEnd_Implementation(AActor* Interactor) override;

protected:
    // Handle replicated state
    UFUNCTION()
    void OnRep_LightState();

    // Toggles light state (called on server)
    void ToggleLight();

    // Sets light visuals
    void UpdateLightVisuals();

    // Tell what to replic.
    virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;
};