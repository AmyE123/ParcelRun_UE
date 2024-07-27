#include "Parcel.h"
#include "DrawDebugHelpers.h"
#include "TimerManager.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"
#include "ThirdPersonCharacter.h"

// Initialize static constants
const float AParcel::MoveSpeed = 300.0f;

AParcel::AParcel()
{
    PrimaryActorTick.bCanEverTick = true;
    bIsPickedUp = false;
    bMoving = false;
    bDelivered = false;
    bDestroyInitiated = false; // Initialize destroy flag
    MoveStartTime = 0.0f;
    MoveDuration = 0.0f;

    // Collision Setup
    CollisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionComponent"));
    CollisionComponent->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
    CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &AParcel::OnOverlapBegin);
    RootComponent = CollisionComponent;

    // Static Mesh Setup
    ParcelMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ParcelMesh"));
    ParcelMesh->SetupAttachment(RootComponent);
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

            if (!bDestroyInitiated)
            {
                bDestroyInitiated = true;
                GetWorld()->GetTimerManager().SetTimer(DestroyTimerHandle, this, &AParcel::DestroyParcel, 2.0f, false);
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
        PickUp(Character);
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

    // Detach from any previous parent and attach to the character's hand socket
    DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

    USkeletalMeshComponent* CharacterMesh = const_cast<USkeletalMeshComponent*>(Character->GetMesh());
    if (CharacterMesh != nullptr)
    {
        bool bAttached = AttachToComponent(CharacterMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("HandSocket"));
        if (!bAttached)
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to attach parcel to %s"), *CharacterMesh->GetName());
        }
        else
        {
            // Keep the parcel at the origin relative to the hand socket
            ParcelMesh->SetRelativeLocation(FVector::ZeroVector);
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
        return;
    }

    if (bIsPickedUp)
    {
        DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
        StartMoveToTarget(TargetLocation);
        bIsPickedUp = false;
    }
}

void AParcel::StartMoveToTarget(FVector TargetLocation)
{
    MoveToLocation = TargetLocation;
    bMoving = true;
    MoveStartTime = GetWorld()->GetTimeSeconds();
    MoveDuration = (TargetLocation - GetActorLocation()).Size() / MoveSpeed;
    bDestroyInitiated = false;
    bDelivered = true;
}

void AParcel::DestroyParcel()
{
    Destroy();
}