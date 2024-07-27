// Fill out your copyright notice in the Description page of Project Settings.

#include "Postbox.h"
#include "Engine/World.h"
#include "TimerManager.h"

// Sets default values
APostbox::APostbox()
{
    PrimaryActorTick.bCanEverTick = true;

    // Initialize the ParcelSpawningPoint
    ParcelSpawningPoint = CreateDefaultSubobject<USceneComponent>(TEXT("ParcelSpawningPoint"));
    RootComponent = ParcelSpawningPoint;
}

void APostbox::BeginPlay()
{
    Super::BeginPlay();
    SpawnParcel();
}

void APostbox::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void APostbox::SpawnParcel()
{
    if (ParcelClass)
    {
        FVector SpawnLocation = ParcelSpawningPoint->GetComponentLocation();
        FRotator SpawnRotation = ParcelSpawningPoint->GetComponentRotation();

        FActorSpawnParameters SpawnParams;
        SpawnParams.Owner = this;
        SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

        AParcel* SpawnedParcel = GetWorld()->SpawnActor<AParcel>(ParcelClass, SpawnLocation, SpawnRotation, SpawnParams);

        if (SpawnedParcel)
        {
            SpawnedParcel->AttachToComponent(ParcelSpawningPoint, FAttachmentTransformRules::KeepWorldTransform);
            SpawnedParcel->OnParcelPickedUp.AddDynamic(this, &APostbox::OnParcelPickedUp);
        }
    }
}

void APostbox::OnParcelPickedUp()
{
    GetWorld()->GetTimerManager().SetTimer(SpawnTimerHandle, this, &APostbox::SpawnParcel, ParcelRespawnRate, false);
}