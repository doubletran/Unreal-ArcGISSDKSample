// Copyright Epic Games, Inc. All Rights Reserved.


#include "FirstPlayerController.h"
#include "FirstPawn.h"
#include "Map/Vehicle.h"
#include "FirstUI.h"
#include "EnhancedInputSubsystems.h"
#include "ChaosWheeledVehicleMovementComponent.h"

void AFirstPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	// spawn the UI widget and add it to the viewport
	VehicleUI = CreateWidget<UFirstUI>(this, VehicleUIClass);

	check(VehicleUI);

	VehicleUI->AddToViewport();
}

void AFirstPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	
	// get the enhanced input subsystem
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		// add the mapping context so we get controls
		UE_LOG(LogTemp, Log, TEXT("UE_LOG in PlayerController"));
		Subsystem->AddMappingContext(InputMappingContext, 0);
	}
}

void AFirstPlayerController::Tick(float Delta)
{
	Super::Tick(Delta);

	if (IsValid(VehiclePawn) && IsValid(VehicleUI))
	{
		VehicleUI->UpdateSpeed(VehiclePawn->GetChaosVehicleMovement()->GetForwardSpeed());
		VehicleUI->UpdateGear(VehiclePawn->GetChaosVehicleMovement()->GetCurrentGear());
	}
}

void AFirstPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	// get a pointer to the controlled pawn
	//VehiclePawn = CastChecked<AFirstPawn>(InPawn);
	VehiclePawn = CastChecked<AVehicle>(InPawn);
	if (VehiclePawn) {
		UE_LOG(LogTemp, Warning, TEXT("VehiclePawn"));
	}
}
