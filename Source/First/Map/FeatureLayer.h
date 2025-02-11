// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Http.h"
#include "Json.h"
#include "ArcGISMapsSDK/Actors/ArcGISMapActor.h"
#include "ArcGISMapsSDK/BlueprintNodes/GameEngine/Geometry/ArcGISGeometryEngine.h"
#include "ArcGISMapsSDK/BlueprintNodes/GameEngine/Geometry/ArcGISSpatialReference.h"
#include "ArcGISMapsSDK/BlueprintNodes/GameEngine/Layers/ArcGIS3DObjectSceneLayer.h"
#include "ArcGISMapsSDK/BlueprintNodes/GameEngine/Layers/Base/ArcGISLayerCollection.h"
#include "ArcGISMapsSDK/Actors/ArcGISActor.h"
#include "Locator.h"
#include "Pin.h"
#include "Kismet/GameplayStatics.h"
#include "FeatureLayer.generated.h"
namespace Esri
{
	namespace GameEngine
	{
		namespace Attributes
		{
			class ArcGISAttribute;
			class ArcGISAttributeProcessor;
		} // namespace Attributes
	} // namespace GameEngine
} // namespace Esri

UCLASS()
class FIRST_API AFeatureLayer : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFeatureLayer();

	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	AArcGISMapActor* MapActor;

	UPROPERTY(BlueprintReadWrite);
	bool bFeatureProcessed = false;

	UPROPERTY(BlueprintReadWrite)
	TArray<int> featureIds;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	UFUNCTION(BlueprintCallable,Category="LayerManage")
	void SetFeatureAttribute(TArray<FHitResult>hits);



private:


	void SendFeatureAmenityQuery();
	void ProcessResponse(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSucessfully);

	void SetMaterial();
	// The class needs to be set in blueprint
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AActor> ActorClass;

	FString APIToken = "AAPK3c142f51052b4e96aa58ce0888da6c5fw7_68KEwMv1Qbef2wWu3Fy1cdzv_xofybaH_Q_EA1MOOa3W4Um1fc1iBbzZS3yH6";
	TSharedPtr<Esri::GameEngine::Attributes::ArcGISAttributeProcessor> AttributeProcessor;

	void ForEachObject(const Esri::GameEngine::Attributes::ArcGISAttribute& attribute, TFunction<void(const int32, int32)> predicate);
	
	int32 IsBuildingOfInterest(const int32 buildingOID);

	

};
