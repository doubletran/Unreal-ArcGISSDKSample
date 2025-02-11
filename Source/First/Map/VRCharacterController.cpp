// Fill out your copyright notice in the Description page of Project Settings.


#include "VRCharacterController.h"
#include "Kismet/KismetMathLibrary.h"
// Sets default values
AVRCharacterController::AVRCharacterController()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	vrOrigin = CreateDefaultSubobject<USceneComponent>(TEXT("VROrigin"));
	vrOrigin->SetupAttachment(RootComponent);
	vrCamera = CreateDefaultSubobject<UArcGISCameraComponent>(TEXT("FollowCamera"));
	vrCamera->SetupAttachment(vrOrigin);
	vrCamera->SetRelativeLocation(FVector(0.0f, 0.0f, -90.0f));

	leftMotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("LeftMotionController"));
	leftMotionController->SetupAttachment(vrOrigin);
	leftMotionController->SetTrackingSource(EControllerHand::Left);

	leftMotionControllerInteractor = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("LeftMotionControllerInteractor"));
	leftMotionControllerInteractor->SetupAttachment(vrOrigin);
	leftMotionControllerInteractor->SetTrackingSource(EControllerHand::Left);
	leftInteraction = CreateDefaultSubobject<UWidgetInteractionComponent>(TEXT("Left Interactor"));
	leftInteraction->SetupAttachment(leftMotionControllerInteractor);


	rightMotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("RightMotionController"));
	rightMotionController->SetupAttachment(vrOrigin);
	rightMotionController->SetTrackingSource(EControllerHand::Right);
	rightMotionControllerInteractor = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("RightMotionControllerInteractor"));
	rightMotionControllerInteractor->SetupAttachment(vrOrigin);
	rightMotionControllerInteractor->SetTrackingSource(EControllerHand::Right);
	rightInteraction = CreateDefaultSubobject<UWidgetInteractionComponent>(TEXT("Right Interactor"));
	rightInteraction->SetupAttachment(rightMotionControllerInteractor);
}

// Called when the game starts or when spawned
void AVRCharacterController::BeginPlay()
{
	Super::BeginPlay();
	GEngine->XRSystem->SetTrackingOrigin(TrackingOrigin);
	GetCharacterMovement()->SetMovementMode(MOVE_Flying, 0);
	InitializeCapsuleHeight();
	APlayerController* MyPlayerController = UGameplayStatics::GetPlayerController(this, 0);
	if (MyPlayerController)
	{
		UE_LOG(LogTemp, Log, TEXT("playercontroller exists"));
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem
			<UEnhancedInputLocalPlayerSubsystem>(MyPlayerController->GetLocalPlayer()))
		{
			UE_LOG(LogTemp, Log, TEXT("Added mappiung context"));
			Subsystem->AddMappingContext(mappingContext, 0);
		}
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("playercontroller not exists"));
	}
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem
			<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			UE_LOG(LogTemp, Log, TEXT("Added mappiung context"));
			Subsystem->AddMappingContext(mappingContext, 0);
		}
	}
	GeoCoder = Cast<AGeocoding>(UGameplayStatics::GetActorOfClass(GetWorld(), AGeocoding::StaticClass()));
	FTimerHandle UpdateHeight;
	GetWorldTimerManager().SetTimer(UpdateHeight, this, &AVRCharacterController::SetCapsuleHeight, 0.35f, true);
	FTimerHandle UpdateCapsuleLocation;
	GetWorldTimerManager().SetTimer(UpdateCapsuleLocation, this, &AVRCharacterController::UpdateRoomScaleMovement, 0.3f, true);
	
}
void AVRCharacterController::SetCapsuleHeight()
{
	FVector DevicePosition;
	FQuat DeviceRotation;
	GEngine->XRSystem->GetCurrentPose(IXRTrackingSystem::HMDDeviceId, DeviceRotation, DevicePosition);
	GetCapsuleComponent()->SetCapsuleSize(GetCapsuleComponent()->GetScaledCapsuleRadius(), capsuleHalfHeight, true);
	auto halfHeight = DevicePosition.Z / 2.0f + 10.0f;
	vrOrigin->AddRelativeLocation(FVector(0.0f, 0.0f, capsuleHalfHeight - halfHeight));
	capsuleHalfHeight = halfHeight;
}
void AVRCharacterController::UpdateRoomScaleMovement()
{
	FVector Offset = vrCamera->GetComponentLocation() - GetActorLocation();
	AddActorWorldOffset(FVector(Offset.X, Offset.Y, 0.0f));
	vrOrigin->AddWorldOffset(UKismetMathLibrary::NegateVector(FVector(Offset.X, Offset.Y, 0.0f)));
}

// Called every frame
void AVRCharacterController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AVRCharacterController::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
	EnhancedInputComponent->BindAction(move_X, ETriggerEvent::Triggered, this, &AVRCharacterController::MoveRight);
	EnhancedInputComponent->BindAction(move_Y, ETriggerEvent::Triggered, this, &AVRCharacterController::MoveForward);
	EnhancedInputComponent->BindAction(move_Z, ETriggerEvent::Triggered, this, &AVRCharacterController::MoveUp);

	EnhancedInputComponent->BindAction(turn, ETriggerEvent::Triggered, this, &AVRCharacterController::Turn);

	//Simulate Mouse Click 
	//EnhancedInputComponent->BindAction(clickLeft, ETriggerEvent::Started, this, &AVRCharacterController::SimulateClickLeft);
	//EnhancedInputComponent->BindAction(clickLeft, ETriggerEvent::Canceled, this, &AVRCharacterController::ResetClickLeft);
	//EnhancedInputComponent->BindAction(clickLeft, ETriggerEvent::Completed, this, &AVRCharacterController::ResetClickLeft);
	EnhancedInputComponent->BindAction(clickRight, ETriggerEvent::Started, this, &AVRCharacterController::SimulateClickRight);
	EnhancedInputComponent->BindAction(clickRight, ETriggerEvent::Canceled, this, &AVRCharacterController::ResetClickRight);
	EnhancedInputComponent->BindAction(clickRight, ETriggerEvent::Completed, this, &AVRCharacterController::ResetClickRight);
	}
}

void AVRCharacterController::MoveUp(const FInputActionValue& value)
{
	auto inputValue = value.Get<float>();

	if (abs(inputValue) > 0.5f)
	{
		AddMovementInput(GetActorUpVector(), inputValue * UpSpeed);
	}
}

void AVRCharacterController::MoveForward(const FInputActionValue& value)
{
	const auto inputValue = value.Get<float>();

	if (abs(inputValue) > MovementDeadzone)
	{
			FVector direction = rightMotionController->GetForwardVector();
			AddMovementInput(direction, inputValue * MoveSpeed);
	}
}
void AVRCharacterController::MoveRight(const FInputActionValue& value)
{
	const auto inputValue = value.Get<float>();
	UE_LOG(LogTemp, Log, TEXT("move right"));
	if (abs(inputValue) > MovementDeadzone)
	{

		FVector direction = rightMotionController->GetRightVector();
		AddMovementInput(direction, inputValue * MoveSpeed);
	}
}
void AVRCharacterController::Turn(const FInputActionValue& value)
{
	auto inputValue = value.Get<float>() * RotationSpeed;
	if (abs(inputValue) > RotationDeadzone)
	{
		SetActorRotation(FRotator(0.0f, GetActorRotation().Yaw + inputValue, 0.0f));
	}
}

void AVRCharacterController::InitializeCapsuleHeight()
{
	capsuleHalfHeight = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
}
void AVRCharacterController::SimulateClickLeft()
{
	leftInteraction->PressPointerKey(EKeys::LeftMouseButton);
}

void AVRCharacterController::SimulateClickRight()
{
	if (rightInteraction->IsOverInteractableWidget())
	{
		rightInteraction->PressPointerKey(EKeys::LeftMouseButton);
	}
	else
	{
		if (GeoCoder != nullptr)
		{
			GeoCoder->SelectLocation(rightInteraction->GetComponentLocation(), rightInteraction->GetForwardVector());
		}
	}
}

void AVRCharacterController::ResetClickLeft()
{
	leftInteraction->ReleasePointerKey(EKeys::LeftMouseButton);
}

void AVRCharacterController::ResetClickRight()
{
	rightInteraction->ReleasePointerKey(EKeys::LeftMouseButton);
}
