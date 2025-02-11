// Copyright Epic Games, Inc. All Rights Reserved.

#include "FirstWheelFront.h"
#include "UObject/ConstructorHelpers.h"

UFirstWheelFront::UFirstWheelFront()
{
	AxleType = EAxleType::Front;
	bAffectedBySteering = true;
	MaxSteerAngle = 40.f;
}