// Fill out your copyright notice in the Description page of Project Settings.


#include "XRTabletopComponent.h"

void UXRTabletopComponent::BeginPlay() {
	Super::BeginPlay();
	if (const auto controller = Cast<ATabletopController>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0)))
	{
		XRTabletopController = controller;
		UE_LOG(LogTemp, Warning, TEXT("Found XR Tabletop Controller"));

	}
	LocalZOffset = WrapperActor->GetRootComponent()->GetRelativeLocation().Z;
	auto relativeLocation = WrapperActor->GetRootComponent()->GetRelativeLocation();
	relativeLocation.Z = GetElevationOffset() *WrapperActor->GetActorRelativeScale3D().Z + LocalZOffset;
	WrapperActor->SetActorRelativeLocation(relativeLocation);
}
