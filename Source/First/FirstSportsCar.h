// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FirstPawn.h"
#include "FirstSportsCar.generated.h"

/**
 *  Sports car wheeled vehicle implementation
 */
UCLASS(abstract)
class FIRST_API AFirstSportsCar : public AFirstPawn
{
	GENERATED_BODY()
	
public:

	AFirstSportsCar();
};
