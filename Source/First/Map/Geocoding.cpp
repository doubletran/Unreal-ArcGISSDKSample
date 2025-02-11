// Fill out your copyright notice in the Description page of Project Settings.
#include "Geocoding.h"


constexpr int DEFAULT_TRACELENGTH= 10000000;
// Sets default values
AGeocoding::AGeocoding()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AGeocoding::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Warning, TEXT("BEGIN"));
	if (ActorClass)
	{
		AActor * actor = UGameplayStatics::GetActorOfClass(
			this,                               // World context object
			ActorClass                          // Actor class
		);
		MapActor = CastChecked<AArcGISMapActor>(actor);
		if (!MapActor) {
			UE_LOG(LogTemp, Warning, TEXT("CAST MAP ACTOR FAILED"));
		}
		else {
			UE_LOG(LogTemp, Warning, TEXT("CAST MAP ACTOR SUCCESS"));
		}
	}
	SetupPlayerInputComponent();
	
	UArcGISMapComponent* MapComponent = UArcGISMapComponent::GetMapComponent(this);
	 APIToken = MapComponent ? MapComponent->GetAPIKey() : "";

	
	
}

void AGeocoding::SetupPlayerInputComponent()
{
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());

	Subsystem->ClearAllMappings();
	Subsystem->AddMappingContext(InputMapping, 0);
	UEnhancedInputComponent* Input = Cast<UEnhancedInputComponent>(PlayerController->InputComponent);
	Input->BindAction(ClickAction, ETriggerEvent::Triggered, this, &AGeocoding::LocateOnClick);

}

void AGeocoding::LocateOnClick(const FInputActionValue& value) {
	FVector WorldLocation;
	FVector WorldDirection;

	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	PlayerController->DeprojectMousePositionToWorld(WorldLocation, WorldDirection);
	SelectLocation(WorldLocation, WorldDirection);

}
void AGeocoding::SelectLocation(FVector location, FVector direction) {
	FHitResult HitResult;
	FActorSpawnParameters SpawnParameters;

	bool bTraceSuccess =
		GetWorld()->LineTraceSingleByChannel(HitResult, location, location + TraceLength * direction, ECC_Visibility, FCollisionQueryParams());
	if (bTraceSuccess) {
		//UE_LOG(LogTemp,Log, TEXT("Hit Face Index: %d"), HitResult.FaceIndex);

		SpawnParameters.Owner = this;
		auto pin = GetWorld()->SpawnActor<APin>(APin::StaticClass(), HitResult.ImpactPoint, FRotator(0), SpawnParameters);
		UArcGISPoint* pinLocation = pin->ArcGISLocation->GetPosition();

		UE_LOG(LogTemp, Log, TEXT("MyVector X =%f"), pinLocation->GetX());
		UE_LOG(LogTemp, Log, TEXT("MyVector Y =%f"), pinLocation->GetY());
		// If the geographic coordinates of the point are not in terms of lat & lon, project them 
		if (pinLocation->GetSpatialReference()->GetWKID() != 4326) {
			UE_LOG(LogTemp, Log, TEXT("Project"));
			auto ProjectedGeometry = UArcGISGeometryEngine::Project(pinLocation,
				UArcGISSpatialReference::CreateArcGISSpatialReference(4326));
			if (ProjectedGeometry != nullptr)
			{
				pinLocation = static_cast<UArcGISPoint*>(ProjectedGeometry);
			}
		}
		SendReverseGeocodingQuery(pinLocation->GetX(), pinLocation->GetY());
	}
}
void AGeocoding::SendReverseGeocodingQuery(float x, float y) {
	FString Url = "https://geocode.arcgis.com/arcgis/rest/services/World/GeocodeServer/reverseGeocode";

	FString Query;
	// Set up the query 
	FHttpRequestRef Request = FHttpModule::Get().CreateRequest();
	Request->OnProcessRequestComplete().BindUObject(this, &AGeocoding::ProcessLocationQueryResponse);
	Query = FString::Printf(TEXT("%s/?f=json&featureTypes=&location=%f,%f&token=%s"), *Url, x, y, *APIToken );
	UE_LOG(LogTemp, Log, TEXT("Query Sent: %s"), *Query);
	Request->SetURL(Query.Replace(TEXT(" "), TEXT("%20")));
	Request->SetVerb("GET");
	Request->SetHeader("Content-Type", "x-www-form-urlencoded");
	Request->ProcessRequest();
	bWaitingForResponse = true;
}

void AGeocoding::ProcessLocationQueryResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSucessfully) {
	FString ResponseAddress = "";
	FString Message;
	TSharedPtr<FJsonValue> Error;
	TSharedPtr<FJsonObject> JsonObj;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());
	UE_LOG(LogTemp, Log,TEXT("Response Received"));
	// Check if the query was successful
	if (FJsonSerializer::Deserialize(Reader, JsonObj) &&
		Response->GetResponseCode() > 199 && Response->GetResponseCode() < 300) {

		TSharedPtr<FJsonValue> AddressField;
		if ((AddressField = JsonObj->TryGetField((TEXT("address"))))) {
			JsonObj = AddressField->AsObject();
			if (!JsonObj->TryGetStringField(TEXT("Match_addr"), ResponseAddress)) {
				ResponseAddress = "Query did not return valid response";
			}
		}
		else if ((Error = JsonObj->TryGetField(TEXT("error")))) {
			JsonObj = Error->AsObject();
			JsonObj->TryGetStringField(TEXT("message"), ResponseAddress);
		}
	}
	UE_LOG(LogTemp, Log,TEXT("%s"), *ResponseAddress);

	bWaitingForResponse = false;
}
// Called every frame
void AGeocoding::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}



