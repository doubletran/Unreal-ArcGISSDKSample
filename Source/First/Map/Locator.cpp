// Fill out your copyright notice in the Description page of Project Settings.


#include "Locator.h"

// Sets default values
ALocator::ALocator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	Root->SetMobility(EComponentMobility::Movable);
	RootComponent = Root;
	ArcGISLocation = CreateDefaultSubobject<UArcGISLocationComponent>(TEXT("ArcGISLocation"));
	ArcGISLocation->SetupAttachment(Root);
}

// Called when the game starts or when spawned
void ALocator::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ALocator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

