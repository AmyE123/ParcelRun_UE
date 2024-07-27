// Fill out your copyright notice in the Description page of Project Settings.

#include "Postbox.h"
#include "Engine/World.h"
#include "TimerManager.h"

// Sets default values
APostbox::APostbox()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    // Initialize the ParcelSpawningPoint
    ParcelSpawningPoint = CreateDefaultSubobject<USceneComponent>(TEXT("ParcelSpawningPoint"));
    RootComponent = ParcelSpawningPoint;
}

// Called when the game starts or when spawned
void APostbox::BeginPlay()
{
    Super::BeginPlay();

    // Spawn the first parcel
    SpawnParcel();
}

// Called every frame
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

        // Draw a debug sphere at the spawn location to ensure it's correct
        DrawDebugSphere(GetWorld(), SpawnLocation, 50.0f, 12, FColor::Red, false, 5.0f);

        FActorSpawnParameters SpawnParams;
        SpawnParams.Owner = this;
        SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

        // Log the postbox and spawning point positions for debugging
        UE_LOG(LogTemp, Log, TEXT("Postbox location: %s, ParcelSpawningPoint location: %s"), *GetActorLocation().ToString(), *SpawnLocation.ToString());

        AParcel* SpawnedParcel = GetWorld()->SpawnActor<AParcel>(ParcelClass, SpawnLocation, SpawnRotation, SpawnParams);

        if (SpawnedParcel)
        {
            // Log the initial position of the spawned parcel
            UE_LOG(LogTemp, Log, TEXT("Parcel spawned at location: %s"), *SpawnedParcel->GetActorLocation().ToString());

            // Optionally, you can attach the parcel to the ParcelSpawningPoint if you need it to stay relative
            SpawnedParcel->AttachToComponent(ParcelSpawningPoint, FAttachmentTransformRules::KeepWorldTransform);

            // Bind the pickup event
            SpawnedParcel->OnParcelPickedUp.AddDynamic(this, &APostbox::OnParcelPickedUp);

            // Log the final position to verify it is correct
            UE_LOG(LogTemp, Log, TEXT("Parcel final location after attachment: %s"), *SpawnedParcel->GetActorLocation().ToString());
        }
    }
}

void APostbox::OnParcelPickedUp()
{
    // Handle respawning the parcel after 2 seconds
    GetWorld()->GetTimerManager().SetTimer(SpawnTimerHandle, this, &APostbox::SpawnParcel, 2.0f, false);
}