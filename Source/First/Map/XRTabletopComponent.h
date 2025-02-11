// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ArcGISTabletopComponent.h"
#include "TabletopController.h"
#include "XRTabletopComponent.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class FIRST_API UXRTabletopComponent : public UArcGISTabletopComponent
{
	GENERATED_BODY()
	virtual void BeginPlay() override;
private:
	ATabletopController* XRTabletopController;

	double LocalZOffset{ 0 };

	
};
