// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "XRGrabbable.h"
#include "Kismet/KismetSystemLibrary.h"
#include "XRGrabber.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FIRST_API UXRGrabber : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UXRGrabber();

	UXRGrabbable* TryGrab();

	bool TryRelease();
	float ClampGrabDistance(const float& Distance);

private:
	bool bIsTracking = false;
	const float MaxGrabbableDistance{ 2000. };
	const float MinGrabbableDistance{ 5. };

	UXRGrabbable* GrabbedComponent;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
