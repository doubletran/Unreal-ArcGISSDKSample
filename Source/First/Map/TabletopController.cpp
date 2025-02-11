// Fill out your copyright notice in the Description page of Project Settings.


#include "TabletopController.h"
#include "XRTabletopComponent.h"

// Sets default values
ATabletopController::ATabletopController()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	auto vrOrigin = CreateDefaultSubobject<USceneComponent>(TEXT("VROrigin"));
	vrOrigin->SetupAttachment(RootComponent);
	auto vrCamera = CreateDefaultSubobject<UArcGISCameraComponent>(TEXT("FollowCamera"));
	vrCamera->SetupAttachment(vrOrigin);
	vrCamera->SetRelativeLocation(FVector(0.0f, 0.0f, -90.0f));


	rightMotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("RightMotionController"));
	rightMotionController->SetupAttachment(vrOrigin);
	rightMotionController->SetTrackingSource(EControllerHand::Right);
	rightInteraction = CreateDefaultSubobject<UWidgetInteractionComponent>(TEXT("Right Interactor"));
	rightInteraction->SetupAttachment(rightMotionController);
	rightMotionController->MotionSource = TEXT("RightAim");
	grabber = CreateDefaultSubobject<UXRGrabber>(TEXT("Grabber"));
	grabber->SetupAttachment(rightMotionController);

}

// Called when the game starts or when spawned
void ATabletopController::BeginPlay()
{
	Super::BeginPlay();
	GEngine->XRSystem->SetTrackingOrigin(TrackingOrigin);

	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem
			<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{

			Subsystem->AddMappingContext(inputContext, 0);
		}
	}
	SetTabletopComponent();
}

// Called every frame
void ATabletopController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ATabletopController::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(move_X, ETriggerEvent::Triggered, this, &ATabletopController::OnSpin);
		EnhancedInputComponent->BindAction(trigger, ETriggerEvent::Triggered, this, &ATabletopController::StartPanning);
		EnhancedInputComponent->BindAction(trigger, ETriggerEvent::Completed, this, &ATabletopController::StopPanning);
		EnhancedInputComponent->BindAction(move_Y, ETriggerEvent::Triggered, this, &ATabletopController::OnZoom);
		//EnhancedInputComponent->BindAction(trigger, ETriggerEvent::Canceled, this, &ATabletopController::);
		EnhancedInputComponent->BindAction(grip, ETriggerEvent::Started, this, &ATabletopController::OnGrip);
		EnhancedInputComponent->BindAction(grip, ETriggerEvent::Completed, this, &ATabletopController::OnGripRelease);
		EnhancedInputComponent->BindAction(grip, ETriggerEvent::Canceled, this, &ATabletopController::OnGripRelease);
	}

}

void ATabletopController::OnGrip(){
	UE_LOG(LogTemp, Warning, TEXT("Gripped"));
	GrabbedComponent = grabber->TryGrab();
}

void ATabletopController::OnGripRelease() {	
	UE_LOG(LogTemp, Warning, TEXT("Grip Released"));
	grabber->TryRelease();
	GrabbedComponent = nullptr;
}
void ATabletopController::OnSpin(const FInputActionValue& value) {
	const auto inputValue = value.Get<float>();
	UE_LOG(LogTemp, Warning, TEXT("Spin value %f"), inputValue);
	FQuat QuatRotation = FQuat(FRotator(0.f, inputValue, 0.f));
	if (GrabbedComponent) {
		GrabbedComponent->GetOwner()->AddActorWorldRotation(QuatRotation, false, 0, ETeleportType::None);
		//GrabbedComponent->AddWorldRotation(QuatRotation, false, 0, ETeleportType::None);
	}
}

void ATabletopController::OnZoom(const FInputActionValue& value) {
	const auto input = value.Get<float>();
	if (bIsPanning) {
		ZoomLevel += value.Get<float>();

		if (FMath::Abs(ZoomLevel) > MinZoomIncrement)
		{
			TabletopComponent->ZoomMap(ZoomLevel);
			ZoomLevel = 0;
		}
	}
}
void ATabletopController::StartPanning() {
	if (bIsPanning) {
		UpdatePanning();
	}
	else {
		auto hitLocation = FVector3d::ZeroVector;
		auto bHitInExtent = TabletopComponent->Raycast(
		rightMotionController->GetComponentLocation(),
		rightMotionController->GetForwardVector(), hitLocation);
	if (bHitInExtent) {
		bIsPanning = true;
		auto currentTransform = FTransform(TabletopComponent->GetMapComponentLocation());
		auto toWorldTransform = TabletopComponent->GetFromEngineTransform();
		FTransform::Multiply(&PanStartWorldTransform, &currentTransform, &toWorldTransform);
		PanStartEnginePos = hitLocation;
	}
	}


}

void ATabletopController::UpdatePanning() {
	auto hitLocation = PanStartEnginePos;
	if (TabletopComponent)
	{
		TabletopComponent->Raycast(rightMotionController->GetComponentLocation(), rightMotionController->GetForwardVector(), hitLocation);
		auto panDeltaEngine = PanStartEnginePos - hitLocation;

		if (FVector3d::Dist(hitLocation, PanLastEnginePos) > MaxEnginePanDistancePerTick)
		{
			panDeltaEngine *= (panDeltaEngine.Length() + MaxEnginePanDistancePerTick) / panDeltaEngine.Length();
		}
		PanLastEnginePos = hitLocation;

		auto panDeltaWorld = PanStartWorldTransform.TransformPosition(panDeltaEngine);
		TabletopComponent->MoveExtentCenter(panDeltaWorld);
	}
}

void ATabletopController::StopPanning() {
	bIsPanning = false;
}
void ATabletopController::SetTabletopComponent() {
	auto mapComponent = UArcGISMapComponent::GetMapComponent(this);
	if (mapComponent)
	{

		TabletopComponent = mapComponent->GetOwner()->FindComponentByClass<UXRTabletopComponent>();	

		UE_LOG(LogTemp, Warning, TEXT("Set tabletopComponent"));
	}
}