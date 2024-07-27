#include "Parcel.h"
#include "DrawDebugHelpers.h"
#include "TimerManager.h"

#include "Components/SkeletalMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Engine/World.h"


AParcel::AParcel()
{
    PrimaryActorTick.bCanEverTick = true;
    bIsPickedUp = false;
    bMoving = false;
    bDelivered = false;
    bDestroyInitiated = false; // Initialize destroy flag
    MoveSpeed = 300.0f;
    MoveStartTime = 0.0f;
    MoveDuration = 0.0f;

    // Collision Setup
    CollisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionComponent"));
    CollisionComponent->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
    CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &AParcel::OnOverlapBegin);
    RootComponent = CollisionComponent;
}

void AParcel::BeginPlay()
{
    Super::BeginPlay();
}

void AParcel::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (bMoving)
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
            bMoving = false;
            //bDelivered = true;

            // Start the timer to destroy the parcel after a delay, if not already initiated
            if (!bDestroyInitiated)
            {
                bDestroyInitiated = true;
                GetWorld()->GetTimerManager().SetTimer(DestroyTimerHandle, this, &AParcel::DestroyParcel, 2.0f, false);
                UE_LOG(LogTemp, Log, TEXT("Parcel will be destroyed in 2 seconds"));
            }
        }
    }
}

void AParcel::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    AThirdPersonCharacter* Character = Cast<AThirdPersonCharacter>(OtherActor);
    if (Character && !bDelivered && !bIsPickedUp && Character->HeldParcel == nullptr)
    {
        bIsPickedUp = true;
        Character->HeldParcel = this;
        PickUp(Character);  // Assuming this is a method that handles the visual and gameplay effects of picking up the parcel
        Character->SelectRandomHouse();
    }
}

void AParcel::PickUp(const AThirdPersonCharacter* Character)
{
    if (Character == nullptr)
    {
        return;
    }

    bIsPickedUp = true;

    // Visually pick up the parcel (& become a child)
    USkeletalMeshComponent* CharacterMesh = const_cast<USkeletalMeshComponent*>(Character->GetMesh());

    if (CharacterMesh != nullptr)
    {
        // Attach the parcel to the HandSocket
        bool bAttached = AttachToComponent(CharacterMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("HandSocket"));
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

    // Notify the postbox that the parcel has been picked up
    OnParcelPickedUp.Broadcast();
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
    bMoving = true;
    MoveStartTime = GetWorld()->GetTimeSeconds();
    MoveDuration = (TargetLocation - GetActorLocation()).Size() / MoveSpeed;   
    bDestroyInitiated = false; // Reset the destroy initiation flag
    bDelivered = true; // Reset the delivered status
}

void AParcel::DestroyParcel()
{
    UE_LOG(LogTemp, Log, TEXT("Parcel destroyed"));
    Destroy();
}