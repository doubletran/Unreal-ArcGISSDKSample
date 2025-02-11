// Fill out your copyright notice in the Description page of Project Settings.


#include "XRGrabber.h"

// Sets default values for this component's properties
UXRGrabber::UXRGrabber()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UXRGrabber::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UXRGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

UXRGrabbable* UXRGrabber::TryGrab() {
	TArray<FHitResult> traceHits;
	TArray<TEnumAsByte<EObjectTypeQuery>>ObjectTypesArray;

	ObjectTypesArray.Add(UEngineTypes::ConvertToObjectType(ECC_Visibility));

	if (UKismetSystemLibrary::LineTraceMultiForObjects(GetWorld(), GetComponentLocation(), GetComponentLocation() + GetForwardVector() * MaxGrabbableDistance, ObjectTypesArray, false, TArray<AActor*>(), EDrawDebugTrace::None, traceHits, true)) {
		for (auto hit : traceHits) {
			if (!hit.GetComponent()->GetAttachParent()) {
				return nullptr;
			}
			if (auto target = Cast<UXRGrabbable>(hit.GetComponent()->GetAttachParent()))
			{
				auto bGrabSucceeded = target->OnGrabbed(this, hit);
				if (bGrabSucceeded)
				{
					GrabbedComponent = target;
				}
				return GrabbedComponent;
				
			}

		}
	}
	return nullptr;
}

bool UXRGrabber::TryRelease()
{
	if (GrabbedComponent)
	{
		GrabbedComponent->OnReleased();
		GrabbedComponent = nullptr;
		return true;
	}
	return false;
}
