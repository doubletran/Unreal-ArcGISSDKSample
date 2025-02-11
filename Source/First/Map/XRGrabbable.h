// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "XRGrabbable.generated.h"


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class FIRST_API UXRGrabbable : public USceneComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UXRGrabbable();


protected:
	// Called when the game starts
	virtual void BeginPlay() override;


public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool OnGrabbed(UXRGrabber* Grabber, const FHitResult& Hit);
	virtual bool OnGrabbed_Implementation(UXRGrabber* Grabber, const FHitResult& Hit);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OnReleased();
	virtual void OnReleased_Implementation();

private:
	bool bIsGrabbed{ false };
	UXRGrabber* CurrentGrabber;
	float GrabDistance;
	FVector3d GrabOffset; 
		
};
