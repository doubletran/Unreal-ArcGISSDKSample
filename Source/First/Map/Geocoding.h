// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Runtime/Engine/Classes/GameFramework/PlayerController.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "ArcGISMapsSDK/Actors/ArcGISActor.h"
#include "Http.h"
#include "Json.h"
#include "ArcGISMapsSDK/Actors/ArcGISMapActor.h"
#include "ArcGISMapsSDK/BlueprintNodes/GameEngine/Geometry/ArcGISGeometryEngine.h"
#include "ArcGISMapsSDK/BlueprintNodes/GameEngine/Geometry/ArcGISSpatialReference.h"

#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Pin.h"
#include "Geocoding.generated.h"

class UInputMappingContext;
class UInputAction;
class AArcGISMapActor;


UCLASS()
class FIRST_API AGeocoding : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AGeocoding();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "EnhancedInput")
	UInputMappingContext* InputMapping;

	UFUNCTION()
	void SetupPlayerInputComponent();

	UFUNCTION()
	void LocateOnClick(const FInputActionValue& value);

	UPROPERTY(EditAnywhere)
	UInputAction* ClickAction;
	
	UPROPERTY(EditAnywhere)
	int TraceLength = 10000000;

	void SelectLocation(FVector location, FVector direction);

private:
	// The class needs to be set in blueprint
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AActor> ActorClass;

	void SendReverseGeocodingQuery(float x, float y);
	

	void ProcessLocationQueryResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSucessfully);

	bool bWaitingForResponse = false;

	FString APIToken;

	AArcGISMapActor* MapActor;


};