// Copyright Alex Hajdu, 2025

#include "RELeverActor.h"

#include "DrawDebugHelpers.h"
#include "Components/AudioComponent.h"
#include "Components/BillboardComponent.h"
#include "Components/LineBatchComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TextRenderComponent.h"
#include "Components/TimelineComponent.h"
#include "Core/REPlayerController.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"

ARELeverActor::ARELeverActor()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	Mesh->SetupAttachment(Root);

	AudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("Audio"));
	AudioComponent->SetupAttachment(Root);
	AudioComponent->bAutoActivate = false;

	// Billboard for identifying if switch has no connections - visible in game too
	BillboardComponent = CreateDefaultSubobject<UBillboardComponent>(TEXT("Billboard"));
	BillboardComponent->SetupAttachment(RootComponent);
	BillboardComponent->SetRelativeScale3D(FVector(3.0f));
	BillboardComponent->bHiddenInGame = false; //?

	// Interaction trigger
	InteractionTrigger = CreateDefaultSubobject<USphereComponent>(TEXT("InteractionTrigger"));
	InteractionTrigger->SetupAttachment(RootComponent);
	InteractionTrigger->InitSphereRadius(250.f);
	InteractionTrigger->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	InteractionTrigger->SetCollisionObjectType(ECC_WorldDynamic);
	InteractionTrigger->SetCollisionResponseToAllChannels(ECR_Ignore);
	InteractionTrigger->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	InteractionTrigger->SetGenerateOverlapEvents(true);

	LeverStateText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("LeverStateText"));
	LeverStateText->SetupAttachment(RootComponent);
	LeverStateText->SetHorizontalAlignment(EHTA_Center);
	LeverStateText->SetWorldSize(50.f);
	LeverStateText->SetTextRenderColor(FColor::Yellow);
	LeverStateText->SetRelativeLocation(FVector(0.f, 0.f, 80.f)); // above the lever

	// To visualize debug lines per actor and not use global one
	LineBatchComponent = CreateDefaultSubobject<ULineBatchComponent>(TEXT("LineBatchComponent"));
	LineBatchComponent->SetupAttachment(RootComponent);
	LineBatchComponent->SetIsVisualizationComponent(true);

	// Simple anim idea
	AnimationTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("AnimationTimeline"));
}

void ARELeverActor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	if (GetWorld() && !GetWorld()->IsGameWorld())	
	{
		UpdateDebugAppearance();
	}
}

bool ARELeverActor::ShouldTickIfViewportsOnly() const
{
	return true;
}

void ARELeverActor::BeginPlay()
{
	Super::BeginPlay();

	if (RotationCurve)
	{
		FOnTimelineFloat ProgressFunction;
		ProgressFunction.BindUFunction(this, FName("HandleTimelineProgress"));

		AnimationTimeline->AddInterpFloat(RotationCurve, ProgressFunction);
		AnimationTimeline->SetLooping(false);

		StartRotation = Mesh->GetRelativeRotation();
		TargetRotation = StartRotation + FRotator(0.f, 0.f, LeverActivatedAngle);
	}

	InteractionTrigger->OnComponentBeginOverlap.AddDynamic(this, &ARELeverActor::OnTriggerBegin);
	InteractionTrigger->OnComponentEndOverlap.AddDynamic(this, &ARELeverActor::OnTriggerEnd);
}

void ARELeverActor::OnTriggerBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                   UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                   const FHitResult& SweepResult)
{
	if (HasAuthority()) // Only server triggers interaction
	{
		if (ACharacter* PlayerChar = Cast<ACharacter>(OtherActor))
		{
			UE_LOG(LogTemp, Log, TEXT("Player entered switch trigger"));

			if (GetClass()->ImplementsInterface(UREInteractableInterface::StaticClass()))
			{
				IREInteractableInterface::Execute_InteractStart(this, PlayerChar);
			}
		}
	}
}

void ARELeverActor::OnTriggerEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (HasAuthority()) // Only server triggers interaction
	{
		if (ACharacter* PlayerChar = Cast<ACharacter>(OtherActor))
		{
			UE_LOG(LogTemp, Log, TEXT("Player leaved switch trigger"));

			if (GetClass()->ImplementsInterface(UREInteractableInterface::StaticClass()))
			{
				IREInteractableInterface::Execute_InteractEnd(this, PlayerChar);
			}
		}
	}
}

void ARELeverActor::InteractStart_Implementation(AActor* Interactor)
{
	if (HasAuthority())
	{
		ToggleSwitch(Interactor);
	}
	else
	{
		Server_ToggleSwitch(Interactor);
	}
}

void ARELeverActor::InteractEnd_Implementation(AActor* Interactor)
{
	if (HasAuthority())
	{
		ToggleSwitch(Interactor);
	}
	else
	{
		Server_ToggleSwitch(Interactor);
	}
}

void ARELeverActor::Server_ToggleSwitch_Implementation(AActor* Interactor)
{
	ToggleSwitch(Interactor);
}

void ARELeverActor::ToggleSwitch(AActor* Interactor)
{
	// Let's execute only if actor exists and implement interface
	if (ConnectedActor && ConnectedActor->Implements<UREInteractableInterface>())
	{
		bIsOn = !bIsOn;

		{
			APlayerController* PC = nullptr;
			if (APawn* Pawn = Cast<APawn>(Interactor))
			{
				PC = Cast<APlayerController>(Pawn->GetController());
			}
			else
			{
				PC = Cast<APlayerController>(Interactor);
			}

			if (HasAuthority())
			{
				LastInteractorPC = PC;
			}
		}

		if (bIsOn)
		{
			IREInteractableInterface::Execute_InteractStart(ConnectedActor, this);
		}
		else
		{
			IREInteractableInterface::Execute_InteractEnd(ConnectedActor, this);
		}

		OnRep_SwitchState();
		OnSwitchToggled.Broadcast(bIsOn);

		if (AudioComponent)
		{
			AudioComponent->Play();
		}
	}

	///@note No time for logs and err. handling :)
}

void ARELeverActor::OnRep_SwitchState()
{
	PlaySwitchAnimation();
	UpdateLeverText();

	if (!LastInteractorPC)
		return;

	APlayerController* LocalPC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (!LocalPC)
		return;

	if (LocalPC->IsLocalController() && LocalPC == LastInteractorPC)
	{
		if (AREPlayerController* REPC = Cast<AREPlayerController>(LocalPC))
		{
			REPC->HandleHudUI(bIsOn, LeverPurpose);
		}
	}
}

void ARELeverActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ARELeverActor, bIsOn);
	DOREPLIFETIME(ARELeverActor, LastInteractorPC);
}

void ARELeverActor::HandleTimelineProgress(float Value) const
{
	if (Mesh)
	{
		FRotator NewRot = UKismetMathLibrary::RLerp(StartRotation, TargetRotation, Value, true);
		Mesh->SetRelativeRotation(NewRot);
	}
}

void ARELeverActor::PlaySwitchAnimation() const
{
	if (AnimationTimeline && RotationCurve)
	{
		bIsOn ? AnimationTimeline->PlayFromStart() : AnimationTimeline->ReverseFromEnd();
	}
}

void ARELeverActor::UpdateLeverAppearance() const
{
	if (Mesh)
	{
		FVector Scale;
		Scale.X = LeverRadius;
		Scale.Y = LeverLength;
		Scale.Z = LeverRadius;
		Mesh->SetRelativeScale3D(Scale);
	}
}

void ARELeverActor::UpdateLeverText() const
{
	const FString StateText = bIsOn ? TEXT("ON") : TEXT("OFF");
	LeverStateText->SetText(FText::FromString(StateText));
	LeverStateText->SetTextRenderColor(bIsOn ? FColor::Green : FColor::Red);
}

void ARELeverActor::UpdateDebugAppearance() const
{
	if (BillboardComponent)
	{
		BillboardComponent->SetVisibility(ConnectedActor ? false : true);
	}

#if WITH_EDITOR
	// Show indicator if there are no connection defined for this switch
	if (LineBatchComponent && ConnectedActor)
	{
		LineBatchComponent->Flush();
		LineBatchComponent->DrawLine(GetActorLocation(), ConnectedActor->GetActorLocation(), FColor::Green, SDPG_World,
		                             2.0f, 0.0f, 0.0f);
	}
#endif
}

void ARELeverActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	UpdateLeverAppearance();
	UpdateDebugAppearance();
	UpdateLeverText();

}
