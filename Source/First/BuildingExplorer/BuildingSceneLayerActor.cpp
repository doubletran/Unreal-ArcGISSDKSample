// Fill out your copyright notice in the Description page of Project Settings.


#include "BuildingSceneLayerActor.h"
#include <Kismet/GameplayStatics.h>
#include "ArcGISMapsSDK/API/GameEngine/ArcGISLoadStatus.h"
#include "ArcGISMapsSDK/API/GameEngine/Layers/ArcGISBuildingSceneLayer.h"
#include "ArcGISMapsSDK/API/GameEngine/Layers/Base/ArcGISLayer.h"
#include "ArcGISMapsSDK/API/GameEngine/Layers/BuildingScene/ArcGISBuildingAttributeFilter.h"
#include "ArcGISMapsSDK/API/GameEngine/Layers/BuildingScene/ArcGISBuildingSceneLayerActiveBuildingAttributeFilterChangedEvent.h"
#include "ArcGISMapsSDK/API/GameEngine/Layers/BuildingScene/ArcGISBuildingSceneLayerAttributeStatistics.h"
#include "ArcGISMapsSDK/API/GameEngine/Layers/BuildingScene/ArcGISBuildingSceneSublayer.h"
#include "ArcGISMapsSDK/API/GameEngine/Layers/BuildingScene/ArcGISBuildingSceneSublayerDiscipline.h"
#include "ArcGISMapsSDK/API/GameEngine/Layers/BuildingScene/ArcGISSolidBuildingFilterDefinition.h"
#include "ArcGISMapsSDK/API/Unreal/ArcGISCollection.h"
#include "ArcGISMapsSDK/API/Unreal/ArcGISDictionary.h"
#include "ArcGISMapsSDK/API/Unreal/ArcGISFuture.h"
#include "ArcGISMapsSDK/API/Unreal/ArcGISImmutableCollection.h"
#include "ArcGISMapsSDK/Actors/ArcGISMapActor.h"
#include "ArcGISMapsSDK/BlueprintNodes/GameEngine/Layers/ArcGISBuildingSceneLayer.h"
#include "ArcGISMapsSDK/BlueprintNodes/GameEngine/Layers/Base/ArcGISLayerCollection.h"
#include "ArcGISMapsSDK/CAPI/GameEngine/Attributes/GEBuildingSceneLayerAttributeStatistics.h"
#include "ArcGISMapsSDK/Components/ArcGISMapComponent.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "GameFramework/Actor.h"
// Sets default values
ABuildingSceneLayerActor::ABuildingSceneLayerActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ABuildingSceneLayerActor::BeginPlay()
{
	APlayerController* const PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	PlayerController->SetInputMode(FInputModeGameAndUI());
	PlayerController->SetShowMouseCursor(true);
	Super::BeginPlay();
	ArcGISMapComponent = UArcGISMapComponent::GetMapComponent(this);
}

// Called every frame
void ABuildingSceneLayerActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

FString ABuildingSceneLayerActor::InitializeBuildingSceneLayer()
{

	
	if (ArcGISMapComponent != nullptr)
			{
		UE_LOG(LogTemp, Log,TEXT("Initializing"));
				const auto& AllLayers = ArcGISMapComponent->GetLayers();

				for (const auto& Layer : AllLayers)
				{
					if (Layer.Type == EArcGISLayerType::ArcGISBuildingSceneLayer)
					{
						BuildingSceneLayer = static_cast<Esri::GameEngine::Layers::ArcGISBuildingSceneLayer*>(Layer.APIObject->APIObject.Get());
						auto LoadStatus = BuildingSceneLayer->GetLoadStatus();
						if (LoadStatus == Esri::GameEngine::ArcGISLoadStatus::Loaded) {
							return TEXT("Loaded");
						}
						else if (LoadStatus == Esri::GameEngine::ArcGISLoadStatus::Loading) {
							return TEXT("Loading");
						}
						else {
							break;
						}
					}
				}
			}
		
	
	return TEXT("");
}

void ABuildingSceneLayerActor::GenerateWhereClause(int32 level, int32 phase, bool bClearLevel, bool bNoLevel) {
	auto Filters = BuildingSceneLayer->GetBuildingAttributeFilters();
	FString BuildingLevelClause = TEXT("('") + FString::FromInt(level) + TEXT("')");
	FString ConstructionPhaseClause = TEXT("('");

	for (int32 i = 0; i <= phase; ++i)
	{
		auto PhaseNum = FString::FromInt(i);
		ConstructionPhaseClause += PhaseNum;
		if (i != phase)
		{
			ConstructionPhaseClause += TEXT("', '");
		}
		else
		{
			ConstructionPhaseClause += TEXT("')");
		}
	}
	BuildingLevelClause = FString::Printf(TEXT("BldgLevel in %s"), *BuildingLevelClause);
	ConstructionPhaseClause = FString::Printf(TEXT("CreatedPhase in %s"), *ConstructionPhaseClause);
	FString WhereClause = *ConstructionPhaseClause;

	if (!bClearLevel)
	{
		WhereClause = FString::Printf(TEXT("%s and %s"), *BuildingLevelClause, *ConstructionPhaseClause);
	}
	if (bNoLevel)
	{
		WhereClause = *ConstructionPhaseClause;
	}

	Esri::GameEngine::Layers::BuildingScene::ArcGISBuildingAttributeFilter* LevelFilter;
	for (auto Filter : Filters)
	{
		if (Filter.GetName() == TEXT("Filter"))
		{
			LevelFilter = &Filter;
			auto solidDefinition = Filter.GetSolidFilterDefinition();
			solidDefinition.SetWhereClause(WhereClause);
			BuildingSceneLayer->SetActiveBuildingAttributeFilter(Filter);
		}
	}
}

void ABuildingSceneLayerActor::SetSublayerVisibility(const Esri::GameEngine::Layers::BuildingScene::ArcGISBuildingSceneSublayer& Sublayer,
	bool bVisible)
{
	const_cast<Esri::GameEngine::Layers::BuildingScene::ArcGISBuildingSceneSublayer&>(Sublayer).SetIsVisible(bVisible);
}

void ABuildingSceneLayerActor::AddDisciplineCategoryData()
{
	if (!BuildingSceneLayer)
	{
		return;
	}
	const auto& FirstLayers = BuildingSceneLayer->GetSublayers();
	for (const auto& FirstSubLayer : FirstLayers)
	{
		if (FirstSubLayer.GetName() == TEXT("Full Model"))
		{
			SetSublayerVisibility(FirstSubLayer, true);
			const auto& SecondLayers = FirstSubLayer.GetSublayers();

			for (const auto& SecondSubLayer : SecondLayers)
			{
				FDiscipline NewDiscipline;
				NewDiscipline.Name = SecondSubLayer.GetName();
				const auto& ThirdLayers = SecondSubLayer.GetSublayers();
				SetSublayerVisibility(SecondSubLayer, true);

				for (const auto& ThirdSubLayer : ThirdLayers)
				{
					FCategory SubCategory;
					SubCategory.Name = ThirdSubLayer.GetName();
					NewDiscipline.Categories.Add(SubCategory);
					SetSublayerVisibility(ThirdSubLayer, true);
				}
				DisciplineCategoryData.Add(NewDiscipline);
			}
		}
		else if (FirstSubLayer.GetName() == TEXT("Overview"))
		{
			SetSublayerVisibility(FirstSubLayer, false);
		}
	}

	//Define the order
	TMap<FString, int32> DisciplineOrder;
	DisciplineOrder.Add(TEXT("Architectural"), 0);
	DisciplineOrder.Add(TEXT("Structural"), 1);
	DisciplineOrder.Add(TEXT("Mechanical"), 2);
	DisciplineOrder.Add(TEXT("Electrical"), 3);
	DisciplineOrder.Add(TEXT("Piping"), 4);

	//Sort the disciplines

}
void ABuildingSceneLayerActor::PopulateSublayerMaps(FString option, bool bVisible)
{
	if (!BuildingSceneLayer)
	{
		return;
	}

	const auto& FirstLayers = BuildingSceneLayer->GetSublayers();
	for (const auto& FirstSubLayer : FirstLayers)
	{
		if (FirstSubLayer.GetName() != TEXT("Full Model"))
		{
			continue;
		}

		const auto& SecondLayers = FirstSubLayer.GetSublayers();
		for (const auto& SecondSubLayer : SecondLayers)
		{
			if (SecondSubLayer.GetName() == option)
			{
				SetSublayerVisibility(SecondSubLayer, bVisible);
				return;
			}

			const auto& ThirdLayers = SecondSubLayer.GetSublayers();
			for (const auto& ThirdSubLayer : ThirdLayers)
			{
				if (ThirdSubLayer.GetName() == option)
				{
					SetSublayerVisibility(ThirdSubLayer, bVisible);
					return;
				}
			}
		}
	}
}