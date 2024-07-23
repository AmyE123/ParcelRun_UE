#include "Parcel.h"

AParcel::AParcel()
{
    PrimaryActorTick.bCanEverTick = true;
    bIsPickedUp = false;
}

void AParcel::BeginPlay()
{
    Super::BeginPlay();
}

void AParcel::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AParcel::PickUp()
{
    bIsPickedUp = true;
    // Additional logic for picking up the parcel
}

void AParcel::Throw(FVector TargetLocation)
{
    if (bIsPickedUp)
    {
        FVector Direction = (TargetLocation - GetActorLocation()).GetSafeNormal();
        float ThrowStrength = 1000.0f; // Adjust as needed

        GetStaticMeshComponent()->SetPhysicsLinearVelocity(Direction * ThrowStrength);
        bIsPickedUp = false;
    }
}