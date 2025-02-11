// Copyright Epic Games, Inc. All Rights Reserved.

#include "FirstWheelRear.h"
#include "UObject/ConstructorHelpers.h"

UFirstWheelRear::UFirstWheelRear()
{
	AxleType = EAxleType::Rear;
	bAffectedByHandbrake = true;
	bAffectedByEngine = true;
}