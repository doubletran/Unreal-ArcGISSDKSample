// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Http.h"
#include "Kismet/GameplayStatics.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "ArcGISMapsSDK/Components/ArcGISMapComponent.h"
#include "ArcGISMapsSDK/BlueprintNodes/GameEngine/Geometry/ArcGISGeometryEngine.h"
#include "ArcGISMapsSDK/BlueprintNodes/GameEngine/Geometry/ArcGISSpatialReference.h"
#include "Json.h"
#include "WeatherQuery.generated.h"
USTRUCT(BlueprintType)
struct FIRST_API FCoordinates
{
	GENERATED_BODY()
public: 
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float Longitude;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float Latitude;
};

USTRUCT(BlueprintType)
struct FIRST_API FWeatherData
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FCoordinates Coordinates;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString Country;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString SkyCondition;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString StationName;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float Temperature;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString Weather;


	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool rain = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool thunder = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool snow = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool cloudy = false;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool clear = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool sunny = false;


};
UCLASS()
class FIRST_API AWeatherQuery : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeatherQuery();
	
	UFUNCTION(BlueprintCallable)
	void ProcessWebRequest();

	UFUNCTION(BlueprintImplementableEvent)
	void SetWeather();

	UFUNCTION(BlueprintCallable)
	void SendCityQuery(float X, float Y);

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	FString CityName;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<FWeatherData>Weather;

	UPROPERTY(BlueprintReadOnly)
	FWeatherData WeatherData; 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess))
	TSubclassOf<class UUserWidget> UIWidgetClass;

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	UUserWidget* UIWidget;

	


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
private:
	void OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnected);
	void ProcessCityQueryResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnected);
	FString webLink = "https://services9.arcgis.com/RHVPKKiFTONKtxq3/ArcGIS/rest/services/NOAA_METAR_current_wind_speed_direction_v1/FeatureServer/0//query?where=COUNTRY+LIKE+%27%25United+States+of+America%27+AND+WEATHER+NOT+IN(%27%2CAutomated+observation+with+no+human+augmentation%3B+there+may+or+may+not+be+significant+weather+present+at+this+time.%27)&outFields=*&f=pgeojson&orderByFields=STATION_NAME";
	FString queryurl = "https://api.weather.gov/gridpoints/PQR/107,118/forecast";
};
