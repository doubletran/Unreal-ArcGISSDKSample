// Fill out your copyright notice in the Description page of Project Settings.


#include "WeatherQuery.h"
#include "Blueprint/UserWidget.h"
// Sets default values
AWeatherQuery::AWeatherQuery()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}


void AWeatherQuery::OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnected) {
	if (bConnected) {
		TSharedPtr<FJsonObject> ResponseObj;
		const auto ResponseBody = Response->GetContentAsString();
		auto Reader = TJsonReaderFactory<>::Create(ResponseBody);

		if (FJsonSerializer::Deserialize(Reader, ResponseObj))
		{


			TArray<TSharedPtr<FJsonValue>>Features = ResponseObj->GetObjectField("properties")->GetArrayField("periods");
			if (!Features.IsEmpty()) {
				TSharedPtr<FJsonValue> WeatherStat = Features[0]; //get the recent eat
				if (!WeatherStat->IsNull())
				{
					auto feature = WeatherStat->AsObject();
					WeatherData.Temperature = feature->GetNumberField("temperature");
					FString forecast = feature->GetStringField("detailedForecast");
					UE_LOG(LogTemp, Warning, TEXT("Forecast : %s"), *forecast);
					if (forecast.Contains("Cloud y") || forecast.Contains("cloudy")) {
						WeatherData.cloudy = true;
					}
					if (forecast.Contains("Rain") || forecast.Contains("Showers")) {
						WeatherData.rain = true;
					}
					if (forecast.Contains("thunder")) {
						WeatherData.thunder = true;
					}
					if (forecast.Contains("clear")) {
						WeatherData.clear = true;
					}
					if (forecast.Contains("unny")) {
						WeatherData.sunny = true;
					}
				}
				WeatherData.rain = true;
				SetWeather();

			}
			else {
				UE_LOG(LogTemp, Warning, TEXT("Received is empty"));
			}

			
		}
	}
}



//Process the request in order to get the data
void AWeatherQuery::ProcessWebRequest()
{
	Weather.Empty();
	FHttpRequestRef Request = FHttpModule::Get().CreateRequest();
	Request->OnProcessRequestComplete().BindUObject(this, &AWeatherQuery::OnResponseReceived);
	Request->SetURL(queryurl);
	Request->SetVerb("Get");
	Request->ProcessRequest();
}

void AWeatherQuery::SendCityQuery(float X, float Y)
{
	FString Url = "https://geocode.arcgis.com/arcgis/rest/services/World/GeocodeServer/reverseGeocode";
	UArcGISMapComponent* MapComponent = UArcGISMapComponent::GetMapComponent(this);
	FString APIToken = MapComponent ? MapComponent->GetAPIKey() : "";
	FString Query;

	// Set up the query 
	FHttpRequestRef Request = FHttpModule::Get().CreateRequest();
	Request->OnProcessRequestComplete().BindUObject(this, &AWeatherQuery::ProcessCityQueryResponse);
	FString latLong = FString::SanitizeFloat(X) + "," + FString::SanitizeFloat(Y);
	Query = FString::Printf(TEXT("%s/?f=json&token=%s&location=%s"), *Url, *APIToken, *latLong);
	Request->SetURL(Query);
	Request->SetVerb("GET");
	Request->ProcessRequest();
	CityName = "";
}

void AWeatherQuery::ProcessCityQueryResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnected)
{
	// Check if the query was successful
	TSharedPtr<FJsonObject> ResponseObj;
	const auto ResponseBody = Response->GetContentAsString();
	auto Reader = TJsonReaderFactory<>::Create(ResponseBody);

	//deserialize the json data received in the http request
	if (FJsonSerializer::Deserialize(Reader, ResponseObj))
	{
		auto address = ResponseObj->GetObjectField("address");
		if (address->GetStringField("City").Len() > 0)
		{
			CityName = address->GetStringField("City") + ", " + address->GetStringField("RegionAbbr");
		}
	}
}

// Called when the game starts or when spawned
void AWeatherQuery::BeginPlay()
{
	Super::BeginPlay();
	ProcessWebRequest();
	
	if (UIWidgetClass != nullptr) {
		UIWidget = CreateWidget<UUserWidget>(GetWorld(), UIWidgetClass);
		if (UIWidget)
		{
			UIWidget->AddToViewport();
			APlayerController* const PlayerController = UGameplayStatics::GetPlayerController(this, 0);
			PlayerController->SetInputMode(FInputModeGameAndUI());
			PlayerController->SetShowMouseCursor(true);
		}
	}
	
	
}



