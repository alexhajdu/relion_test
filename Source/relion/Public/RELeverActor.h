// Copyright Alex Hajdu, 2025

#pragma once

#include "CoreMinimal.h"
#include "REInteractableInterface.h"
#include "GameFramework/Actor.h"
#include "RELeverActor.generated.h"

// Fwd decl.
class USphereComponent;
class AActor;
class USceneComponent;
class UStaticMeshComponent;
class UAudioComponent;
class UStaticMesh;
class UAnimSequence;
class UTimelineComponent;
class UCurveFloat;
class UBillboardComponent;
class ULineBatchComponent;
class APlayerController;
class UTextRenderComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSwitchToggled, bool, bNewState);

///@note Using RE prefix to easy identify project related classes and types

UCLASS()
class RELION_API ARELeverActor : public AActor, public IREInteractableInterface
{
	GENERATED_BODY()

public:
	ARELeverActor();
	virtual void Tick(float DeltaSeconds) override;
	virtual bool ShouldTickIfViewportsOnly() const override;

	// Interface Implementation
	virtual void InteractStart_Implementation(AActor* Interactor) override;
	virtual void InteractEnd_Implementation(AActor* Interactor) override;
	
	///@note Using new TObjectPtr instead of raw ptr; no nullptr init needed
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USceneComponent> Root;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> Mesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UAudioComponent> AudioComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UBillboardComponent> BillboardComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USphereComponent> InteractionTrigger;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UTextRenderComponent> LeverStateText;

	UPROPERTY()
	TObjectPtr<ULineBatchComponent> LineBatchComponent;

	UPROPERTY(ReplicatedUsing = OnRep_SwitchState, BlueprintReadOnly)
	bool bIsOn = false;

	///@note I could bring more config for designers, but time limit

	UPROPERTY(EditAnywhere, Category="Switch|Config")
	FText LeverPurpose = INVTEXT("Please override this text; eg. call elevator..");
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Switch|Appearance", meta=(ClampMin="0.1", ClampMax="10.0"))
	float LeverLength = 5.0f;

	UPROPERTY(EditAnywhere, Category="Switch|Appearance", meta=(ClampMin="0.1", ClampMax="10.0"))
	float LeverRadius = 0.2f;

	UPROPERTY(EditAnywhere, Category="Switch|Appearance", meta=(ClampMin="0.1", ClampMax="10.0"))
	float LeverActivatedAngle = -45.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Switch")
	TObjectPtr<AActor> ConnectedActor;

	UPROPERTY()
	TObjectPtr<UTimelineComponent> AnimationTimeline;

	UPROPERTY(EditAnywhere, Category="Animation")
	TObjectPtr<UCurveFloat> RotationCurve;
	
	// Events
	UPROPERTY(BlueprintAssignable, Category="Switch")
	FOnSwitchToggled OnSwitchToggled;

protected:
	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform& Transform) override;

private:
	UFUNCTION(Server, Reliable)
	void Server_ToggleSwitch(AActor* Interactor);

	UFUNCTION()
	void OnRep_SwitchState();

	UFUNCTION()
	void HandleTimelineProgress(float Value) const;

	UFUNCTION()
	void OnTriggerBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnTriggerEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void PlaySwitchAnimation() const;
	void ToggleSwitch(AActor* Interactor);
	void UpdateDebugAppearance() const;
	void UpdateLeverAppearance() const;
	void UpdateLeverText() const;
	
	UPROPERTY(Replicated)
	TObjectPtr<APlayerController> LastInteractorPC;

	FRotator StartRotation;
	FRotator TargetRotation;
};
