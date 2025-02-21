// Fill out your copyright notice in the Description page of Project Settings.


#include "FeatureLayer.h"
#include <assert.h>
#include "ArcGISMapsSDK/API/GameEngine/Attributes/ArcGISAttribute.h"
#include "ArcGISMapsSDK/API/GameEngine/Attributes/ArcGISAttributeProcessor.h"
#include "ArcGISMapsSDK/API/GameEngine/Attributes/ArcGISVisualizationAttribute.h"
#include "ArcGISMapsSDK/API/GameEngine/Attributes/ArcGISVisualizationAttributeDescription.h"
#include "ArcGISMapsSDK/API/GameEngine/Attributes/ArcGISVisualizationAttributeType.h"
#include "ArcGISMapsSDK/API/GameEngine/Layers/ArcGIS3DObjectSceneLayer.h"
#include "ArcGISMapsSDK/API/Unreal/ArcGISArrayBuilder.h"
#include "ArcGISMapsSDK/API/Unreal/ArcGISImmutableArray.h"


constexpr int TraceLength = 100000;
// Sets default values
AFeatureLayer::AFeatureLayer()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	if (ActorClass)
	{
		AActor* actor = UGameplayStatics::GetActorOfClass(
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

}

// Called when the game starts or when spawned
void AFeatureLayer::BeginPlay()
{
	Super::BeginPlay();
	SendFeatureAmenityQuery();

	
}

// Called every frame
void AFeatureLayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AFeatureLayer::SendFeatureAmenityQuery() {
	
	FHttpRequestRef Request = FHttpModule::Get().CreateRequest();
	UE_LOG(LogTemp, Log, TEXT("SENDING"));
	FString Query = "https://services6.arcgis.com/Do88DoK2xjTUCXd1/arcgis/rest/services/OSM_NA_Amenities/FeatureServer/0/query?f=geojson&where=amenity=%27restaurant%27&outfields=*&geometryType=esriGeometryEnvelope&geometry=-74.2591,40.4774,-73.7004,40.9176&resultRecordCount=50";
	Request->OnProcessRequestComplete().BindUObject(this, &AFeatureLayer::ProcessResponse);
	float xmin = -74.2591f;
	float ymin = 40.4774;
	float xmax = -73.7004;
	float ymax = 40.9176;
	UE_LOG(LogTemp, Log, TEXT("Query Sent: %s"), *Query);
	Request->SetURL(Query.Replace(TEXT(" "), TEXT("%20")));
	Request->SetVerb("GET");
	Request->SetHeader("Content-Type", "x-www-form-urlencoded");
	Request->ProcessRequest();

}

void AFeatureLayer::ProcessResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSucessfully) {
	FString ResponseAddress = "";
	FString Message;
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Owner = this;
	UE_LOG(LogTemp, Log, TEXT("Response Received"));
	FVector SpawnLocation(0.0f, 0.0f, 0.0f);
	// Check if the query was successful

	if (bConnectedSucessfully) {
		TSharedPtr<FJsonObject> ResponseObj;
		const auto ResponseBody = Response->GetContentAsString();
		auto Reader = TJsonReaderFactory<>::Create(ResponseBody);

		if (FJsonSerializer::Deserialize(Reader, ResponseObj)) {
			TArray<TSharedPtr<FJsonValue>> Features = ResponseObj->GetArrayField("features");

			for (int i = 0; i != Features.Num(); ++i) {
				auto feature = Features[i]->AsObject();
				auto type = feature->GetObjectField("geometry")->GetStringField("type");
				//this will get the geometry or coordinates of the feature
				auto coordinates = feature->GetObjectField("geometry")->GetArrayField("coordinates");
				float x = coordinates[0]->AsNumber();
				float y = coordinates[1]->AsNumber();
				auto pin = GetWorld()->SpawnActor<APin>(APin::StaticClass(),SpawnLocation, FRotator(0), SpawnParameters);
				
				UArcGISPoint* pinLocation = UArcGISPoint::CreateArcGISPointWithXYZSpatialReference(x, y,100, UArcGISSpatialReference::CreateArcGISSpatialReference(4326));
				UE_LOG(LogTemp, Log, TEXT("MyVector X =%f"), pinLocation->GetX());
				UE_LOG(LogTemp, Log, TEXT("MyVector Y =%f"), pinLocation->GetY());
				UE_LOG(LogTemp, Log, TEXT("MyVector Z =%f"), pinLocation->GetZ());
				pin->ArcGISLocation->SetPosition(pinLocation);
				FVector location = pin->GetRootComponent()->GetComponentLocation();
				FVector groundLocation = location;
			}
		}
	}
	bFeatureProcessed = true;
	
}
void AFeatureLayer::SetMaterial() {

	UArcGISMap* MapComponent = UArcGISMapComponent::GetMapComponent(this)->GetMap();
	UArcGISLayerCollection* layers = MapComponent->GetLayers();
	layers->Remove(0);

	auto layer = UArcGIS3DObjectSceneLayer::CreateArcGIS3DObjectSceneLayerWithProperties("https://tiles.arcgis.com/tiles/P3ePLMYs2RVChkJx/arcgis/rest/services/Buildings_NewYork_17/SceneServer", "NYScene", 1.0f, true, APIToken);


	auto layerAttributes = Esri::Unreal::ArcGISImmutableArray<FString>::CreateBuilder();

	/*layerAttributes.Add("CNSTRCT_YR");

	auto layerAPIObject = StaticCastSharedPtr<Esri::GameEngine::Layers::ArcGIS3DObjectSceneLayer>(layer->APIObject);
	layerAPIObject->SetAttributesToVisualize(layerAttributes.MoveToArray());

	// We want to set the material we will use to visualize this layer
	// In Unreal Engine, open this material in the Material Editor to view the shader graph
	// In general, you can use this function in other scripts to change the material thats used to render the buildings
	layer->SetMaterialReference(
		LoadObject<UMaterial>(this, TEXT("Material'/Game/SampleViewer/Samples/MaterialByAttribute/Materials/ConstructionYearRenderer.ConstructionYearRenderer'")));

	MapComponent->GetLayers()->Add(layer);
	*/
	layerAttributes.Add("OBJECTID");
	auto attributeDescriptions =
		Esri::Unreal::ArcGISImmutableArray<Esri::GameEngine::Attributes::ArcGISVisualizationAttributeDescription>::CreateBuilder();
	attributeDescriptions.Add(Esri::GameEngine::Attributes::ArcGISVisualizationAttributeDescription(
		"IsBuildingOfInterest", Esri::GameEngine::Attributes::ArcGISVisualizationAttributeType::Int32));

	AttributeProcessor = ::MakeShared<Esri::GameEngine::Attributes::ArcGISAttributeProcessor>();
	AttributeProcessor->SetProcessEvent(
		[this](const Esri::Unreal::ArcGISImmutableArray<Esri::GameEngine::Attributes::ArcGISAttribute>& inputAttributes,
			const Esri::Unreal::ArcGISImmutableArray<Esri::GameEngine::Attributes::ArcGISVisualizationAttribute>& outputVisualizationAttributes) {


				// If layerAttributes contained an additional element, it would be at inputAttributes.At(1)
				const auto objectAttribute = inputAttributes.At(0);

				// The outputVisualizationAttributes array expects that its data is indexed the same way as the attributeDescriptions above.
				const auto isBuildingOfInterestAttribute = outputVisualizationAttributes.At(0);

				const auto isBuildingOfInterestBuffer = isBuildingOfInterestAttribute.GetData();
				const auto isBuildingOfInterestData =
					TArrayView<int32>(reinterpret_cast<int32*>(isBuildingOfInterestBuffer.GetData()), isBuildingOfInterestBuffer.Num() / sizeof(int32));
				auto size = isBuildingOfInterestBuffer.Num() / sizeof(int32);
				// Go over each attribute and if its name is one of the four buildings of interest
// It sets a "isBuildingOfInterest" value to 1, otherwise it is set to 0
				ForEachObject(objectAttribute, [this, &isBuildingOfInterestData](const int32 element, int32 index) {
					isBuildingOfInterestData[index] = IsBuildingOfInterest(element);
					});

		});

	// Pass the layer attributes, attribute descriptions and the attribute processor to the layer
	auto layerAPIObject = StaticCastSharedPtr<Esri::GameEngine::Layers::ArcGIS3DObjectSceneLayer>(layer->APIObject);
	layerAPIObject->SetAttributesToVisualize(layerAttributes.MoveToArray(), attributeDescriptions.MoveToArray(), *AttributeProcessor);

	// In Unreal Engine, open this material in the Material Editor to view the shader graph
	// In general, you can use this function in other scripts to change the material thats used to render the buildings
	layer->SetMaterialReference(
		LoadObject<UMaterial>(this, TEXT("Material'/Game/SampleViewer/FeatureQuery/Materials/BuildingNameRenderer.BuildingNameRenderer'")));
	MapComponent->GetLayers()->Add(layer);
}

void AFeatureLayer::SetFeatureAttribute(TArray<FHitResult>hits)
{
	UArcGISMapComponent* MapComponent = UArcGISMapComponent::GetMapComponent(this);
	for (const auto& hit: hits) {
		auto ArcGISHit = MapComponent->GetArcGISRaycastHit(hit);
		UE_LOG(LogTemp, Log, TEXT("ObjectId: %d"), ArcGISHit.FeatureId);
		featureIds.Add(ArcGISHit.FeatureId);
	}
	SetMaterial();
}
// ForEachOID iterates over an array of OID (Object IDs) in a buffer and applies a predicate to each OID.
void AFeatureLayer::ForEachObject(const Esri::GameEngine::Attributes::ArcGISAttribute & attribute,
	TFunction<void(const int32, int32)> predicate)
	{
		// Get the raw buffer data for the attribute
		const auto buffer = attribute.GetData();
		const auto bufferData =
			TArrayView<int32>(reinterpret_cast<int32*>(buffer.GetData()), buffer.Num() / sizeof(int32));
		
		int count = buffer.Num() / sizeof(int32);
		//UE_LOG(LogTemp, Warning, TEXT("Count input: %d"), );
		for (int i = 0; i < count; ++i) {
			predicate(bufferData[i], i);
		}
		
	}

	// This function checks if the building contains a name we are interested in visualizing
	int32 AFeatureLayer::IsBuildingOfInterest(const int32 buildingOID)
	{
		
		if (featureIds.Contains(buildingOID)) {
			UE_LOG(LogTemp, Warning, TEXT("%d"), buildingOID);
			return 1;
		}
	
		return 0;
	
	}












































