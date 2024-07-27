#include "Postbox.h"
#include "Parcel.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"
#include "TimerManager.h"

// Sets default values
APostbox::APostbox()
{
    PrimaryActorTick.bCanEverTick = true;

    // Initialize the StaticMesh component for the postbox
    PostboxMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PostboxMesh"));
    RootComponent = PostboxMesh;

    // Initialize the ParcelSpawningPoint
    ParcelSpawningPoint = CreateDefaultSubobject<USceneComponent>(TEXT("ParcelSpawningPoint"));
    ParcelSpawningPoint->SetupAttachment(PostboxMesh);  // Attach it to the PostboxMesh
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
            // Attach to the ParcelSpawningPoint
            SpawnedParcel->AttachToComponent(ParcelSpawningPoint, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
            SpawnedParcel->OnParcelPickedUp.AddDynamic(this, &APostbox::OnParcelPickedUp);
        }
    }
}

void APostbox::OnParcelPickedUp()
{
    GetWorld()->GetTimerManager().SetTimer(SpawnTimerHandle, this, &APostbox::SpawnParcel, ParcelRespawnRate, false);
}