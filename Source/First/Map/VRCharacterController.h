// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnhancedInputComponent.h"
#include "ArcGISMapsSDK/Components/ArcGISCameraComponent.h"
#include "ArcGISMapsSDK/Components/ArcGISLocationComponent.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "InputCoreTypes.h"
#include "MotionControllerComponent.h"
#include "Components/WidgetInteractionComponent.h"
#include "Components/CapsuleComponent.h"
#include "IXRTrackingSystem.h"
#include "Geocoding.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "VRCharacterController.generated.h"

UCLASS()
class FIRST_API AVRCharacterController : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AVRCharacterController();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
	float MovementDeadzone = 0.2f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
	int MoveSpeed = 1000000000;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
	float RotationDeadzone = 0.2f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
	float RotationSpeed = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
	int SnapRotationDegrees = 30;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables")
	int UpSpeed = 100000;
	AGeocoding* GeoCoder;
	float capsuleHalfHeight;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
private:
	void MoveForward(const FInputActionValue& value);
	void MoveRight(const FInputActionValue& value);
	void MoveUp(const FInputActionValue& value);
	void Turn(const FInputActionValue& value);
	void SimulateClickRight();
	void SimulateClickLeft();
	void ResetClickRight();
	void ResetClickLeft();

	void UpdateRoomScaleMovement();
	// set up motion controller
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables", meta = (AllowPrivateAccess = "true"))
	UMotionControllerComponent* leftMotionController;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables", meta = (AllowPrivateAccess = "true"))
	UMotionControllerComponent* leftMotionControllerInteractor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables", meta = (AllowPrivateAccess = "true"))
	UWidgetInteractionComponent* leftInteraction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables", meta = (AllowPrivateAccess = "true"))
	UWidgetInteractionComponent* rightInteraction;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables", meta = (AllowPrivateAccess = "true"))
	UMotionControllerComponent* rightMotionController;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables", meta = (AllowPrivateAccess = "true"))
	UMotionControllerComponent* rightMotionControllerInteractor;
	TEnumAsByte<EHMDTrackingOrigin::Type> TrackingOrigin;
	UInputMappingContext* mappingContext = LoadObject<UInputMappingContext>(nullptr, TEXT("InputMappingContext'/Game/SampleViewer/VR/Input/IMC_Default.IMC_Default'"));
	UInputAction* move_X = LoadObject<UInputAction>(nullptr, TEXT("InputAction'/Game/SampleViewer/VR/Input/IA_MoveX.IA_MoveX'"));
	UInputAction* move_Y = LoadObject<UInputAction>(nullptr, TEXT("InputAction'/Game/SampleViewer/VR/Input/IA_MoveY.IA_MoveY'"));
	UInputAction* move_Z = LoadObject<UInputAction>(nullptr, TEXT("InputAction'/Game/SampleViewer/VR/Input/IA_MoveUp.IA_MoveUp'"));
	UInputAction* turn = LoadObject<UInputAction>(nullptr, TEXT("InputAction'/Game/SampleViewer/VR/Input/IA_Turn.IA_Turn'"));

	UInputAction* clickRight = LoadObject<UInputAction>(nullptr, TEXT("InputAction'/Game/SampleViewer/VR/Input/IA_Menu_Cursor_Right.IA_Menu_Cursor_Right'"));

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables", meta = (AllowPrivateAccess = "true"))
	UArcGISCameraComponent* vrCamera;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables", meta = (AllowPrivateAccess = "true"))
	USceneComponent* vrOrigin;
	void InitializeCapsuleHeight();
	void SetCapsuleHeight();
	


};
