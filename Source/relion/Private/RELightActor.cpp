// Copyright Alex Hajdu, 2025

#include "RELightActor.h"

#include "Components/PointLightComponent.h"
#include "Net/UnrealNetwork.h"

ARELightActor::ARELightActor()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	PointLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("PointLight"));
	PointLight->SetupAttachment(RootComponent);
}

void ARELightActor::BeginPlay()
{
	Super::BeginPlay();
	UpdateLightVisuals();
}

void ARELightActor::InteractStart_Implementation(AActor* Interactor)
{
	// Only the server changes state
	if (HasAuthority())
	{
		ToggleLight();
	}
}

void ARELightActor::InteractEnd_Implementation(AActor* Interactor)
{
	// Only the server changes state
	if (HasAuthority())
	{
		ToggleLight();
	}
}

void ARELightActor::ToggleLight()
{
	bIsOn = !bIsOn;

	// Update visuals
	UpdateLightVisuals();

	// Notify clients
	OnRep_LightState();
}

void ARELightActor::OnRep_LightState()
{
	UpdateLightVisuals();
}

void ARELightActor::UpdateLightVisuals()
{
	if (PointLight)
	{
		PointLight->SetVisibility(bIsOn);
		PointLight->SetLightColor(bIsOn ? LightOnColor : LightOffColor);
	}
}

void ARELightActor::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ARELightActor, bIsOn);
}