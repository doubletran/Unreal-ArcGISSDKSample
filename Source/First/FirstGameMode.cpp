// Copyright Epic Games, Inc. All Rights Reserved.

#include "FirstGameMode.h"
#include "FirstPlayerController.h"

AFirstGameMode::AFirstGameMode()
{
	PlayerControllerClass = AFirstPlayerController::StaticClass();
}
