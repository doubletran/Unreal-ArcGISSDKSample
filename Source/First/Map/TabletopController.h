// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "GameFramework/Actor.h"
#include "MotionControllerComponent.h"
#include "Components/WidgetInteractionComponent.h"

#include "ArcGISMapsSDK/Components/ArcGISCameraComponent.h"
#include "ArcGISMapsSDK/Components/ArcGISLocationComponent.h"

#include "HeadMountedDisplayTypes.h"
#include "IXRTrackingSystem.h"

#include "XRGrabber.h"
#include "XRGrabbable.h"
#include "GameFramework/Pawn.h"

#include "TabletopController.generated.h"
class UXRTabletopComponent;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class FIRST_API ATabletopController : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ATabletopController();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess))
	UXRTabletopComponent* TabletopComponent;
private:
	bool bIsPanning{ false };

	const double MaxEnginePanDistancePerTick{ 5. };
	const float MinZoomIncrement{ 4. };
	float ZoomLevel;

	UInputAction* grip = LoadObject<UInputAction>(nullptr, TEXT("InputAction'/Game/Map/XRTableTop/Input/IA_Grip_Right.IA_Grip_Right'"));
	UInputAction *trigger = LoadObject<UInputAction>(nullptr, TEXT("InputAction'/Game/Map/XRTableTop/Input/IA_Pan_Right.IA_Pan_Right'"));
	UInputAction* spin = LoadObject<UInputAction>(nullptr, TEXT("InputAction'/Game/Map/XRTableTop/Input/IA_Spin.IA_Spin'"));
	UInputAction* move_X = LoadObject<UInputAction>(nullptr, TEXT("InputAction'/Game/Map/VR/Input/IA_MoveX.IA_MoveX'"));
	UInputAction* move_Y = LoadObject<UInputAction>(nullptr, TEXT("InputAction'/Game/Map/VR/Input/IA_MoveY.IA_MoveY'"));
	UInputMappingContext* inputContext = LoadObject<UInputMappingContext>(nullptr, TEXT("InputMappingContext'/Game/Map/XRTableTop/Input/IAC_XRTableTop.IAC_XRTableTop'"));


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables", meta = (AllowPrivateAccess = "true"))
	UWidgetInteractionComponent* rightInteraction;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables", meta = (AllowPrivateAccess = "true"))
	UMotionControllerComponent* rightMotionController;

	UXRGrabbable* GrabbedComponent;
	UXRGrabber* grabber;
	TEnumAsByte<EHMDTrackingOrigin::Type> TrackingOrigin;
	/*EVENTs*/
	void OnGrip();
	void OnGripRelease();
	void OnZoom(const FInputActionValue& value);
	/*
	void OnTrigger();
	void OnTriggerRelease();
	*/
	void OnSpin(const FInputActionValue& value);
	void StartPanning();
	void UpdatePanning();
	void StopPanning();
	FVector3d PanLastEnginePos{ FVector3d::ZeroVector };
	FVector3d PanStartEnginePos{ FVector3d::ZeroVector };
	FTransform PanStartWorldTransform{ FTransform::Identity };
	void SetTabletopComponent();


};
