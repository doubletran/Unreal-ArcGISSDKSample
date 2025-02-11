// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "BuildingSceneLayerActor.generated.h"
USTRUCT(BlueprintType)
struct FCategory
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	FString Name;
};

USTRUCT(BlueprintType)
struct FDiscipline
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	FString Name;

	UPROPERTY(BlueprintReadWrite)
	TArray<FCategory> Categories;
};

class UArcGISMapComponent;
namespace Esri
{
	namespace GameEngine
	{
		namespace Layers
		{
			class ArcGISBuildingSceneLayer;
			namespace BuildingScene
			{
				class ArcGISBuildingAttributeFilter;
				class ArcGISBuildingSceneSublayer;
			} // namespace BuildingScene
		} // namespace Layers
	} // namespace GameEngine
} // namespace Esri

UCLASS()
class FIRST_API ABuildingSceneLayerActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABuildingSceneLayerActor();
	

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;



	UFUNCTION(BlueprintCallable, Category = "BuildingSceneLayer")
	void GenerateWhereClause(int32 level, int32 phase, bool bClearLevel, bool bNoLevel);

	UFUNCTION(BlueprintCallable, Category = "BuildingSceneLayer")
	FString InitializeBuildingSceneLayer();

	UFUNCTION(BlueprintCallable, Category = "BuildingSceneLayer")
	void AddDisciplineCategoryData();
	/*
	UFUNCTION(BlueprintCallable, Category = "BuildingSceneLayer")
	UArcGISBuildingSceneLayer* NewBuildingSceneLayer(FString source, FString APIKey);



	UFUNCTION(BlueprintCallable, Category = "BuildingSceneLayer")
	FString LoadStatus();

	UFUNCTION(BlueprintCallable, Category = "BuildingSceneLayer")
	void ConfigureNewBSL(UArcGISBuildingSceneLayer* newLayer);


	*/
	UPROPERTY(BlueprintReadWrite, Category = "BuildingSceneLayer")
	TArray<FDiscipline> DisciplineCategoryData;
	UFUNCTION(BlueprintCallable, Category = "BuildingSceneLayer")
	void PopulateSublayerMaps(FString option, bool bVisible);

private:

	UPROPERTY()
	TWeakObjectPtr<UArcGISMapComponent> ArcGISMapComponent;

	Esri::GameEngine::Layers::ArcGISBuildingSceneLayer* BuildingSceneLayer;

	Esri::GameEngine::Layers::ArcGISBuildingSceneLayer* LastActiveBSL;

	void SetSublayerVisibility(const Esri::GameEngine::Layers::BuildingScene::ArcGISBuildingSceneSublayer& Sublayer, bool bVisible);

};
