#include "Parcel.h"
#include "DrawDebugHelpers.h"

AParcel::AParcel()
{
    PrimaryActorTick.bCanEverTick = true;
    bIsPickedUp = false;
}

void AParcel::BeginPlay()
{
    Super::BeginPlay();
}

//void AParcel::Tick(float DeltaTime)
//{
//    Super::Tick(DeltaTime);
//}

void AParcel::PickUp(const AThirdPersonCharacter* Character)
{
    if (Character == nullptr)
    {
        return; // Ensure the Character is valid
    }

    bIsPickedUp = true;
    
    // Visually pick up the parcel (become a child!!!)
    // Get the non-const pointer to the skeletal mesh component
    USkeletalMeshComponent* CharacterMesh = const_cast<USkeletalMeshComponent*>(Character->GetMesh());
    if (CharacterMesh != nullptr)
    {
        // Try to attach the parcel to the HandSocket
        bool bAttached = AttachToComponent(CharacterMesh, FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("HandSocket"));
        if (bAttached)
        {
            UE_LOG(LogTemp, Log, TEXT("Parcel successfully attached to %s"), *CharacterMesh->GetName());
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to attach parcel to %s"), *CharacterMesh->GetName());
        }
    }

    else
    {
        UE_LOG(LogTemp, Error, TEXT("Character mesh component is null"));
    }

    // Optional: Draw debug sphere at the socket location to visually confirm the socket's position
    if (CharacterMesh)
    {
        FVector SocketLocation = CharacterMesh->GetSocketLocation(TEXT("HandSocket"));
        DrawDebugSphere(GetWorld(), SocketLocation, 10.0f, 32, FColor::Red, false, 10.0f);
    }   
}

void AParcel::Throw(FVector TargetLocation, const AThirdPersonCharacter* Character)
{
    if (Character == nullptr)
    {
        return; // Ensure the Character is valid
    }

    if (bIsPickedUp)
    {
        // Detach the parcel from the character
        DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

        // Start a coroutine or a tick-based function to move towards the target
        StartMoveToTarget(TargetLocation);
        bIsPickedUp = false; // Reset the pick-up status
    }
}

void AParcel::StartMoveToTarget(FVector TargetLocation)
{
    MoveToLocation = TargetLocation;
    Moving = true;
    MoveSpeed = 1000.0f; // Set this speed to your preference for how fast it moves
    MoveStartTime = GetWorld()->GetTimeSeconds();
    MoveDuration = (TargetLocation - GetActorLocation()).Size() / MoveSpeed;
}

void AParcel::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (Moving)
    {
        float CurrentTime = GetWorld()->GetTimeSeconds() - MoveStartTime;
        if (CurrentTime < MoveDuration)
        {
            FVector NewLocation = FMath::Lerp(GetActorLocation(), MoveToLocation, CurrentTime / MoveDuration);
            SetActorLocation(NewLocation);
        }
        else
        {
            SetActorLocation(MoveToLocation);
            Moving = false; // Stop moving
        }
    }
}