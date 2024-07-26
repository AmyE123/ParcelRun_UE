#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "ThirdPersonCharacter.h"
#include "Parcel.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FParcelPickedUpDelegate);

UCLASS()
class AMY_TECHTEST_API AParcel : public AStaticMeshActor
{
    GENERATED_BODY()

public:
    /// <summary>
    /// Delegate to notify when the parcel is picked up.
    /// </summary>
    FParcelPickedUpDelegate OnParcelPickedUp;

    // The parcel's constructor.
    AParcel();

    // Tick function override from AActor.
    virtual void Tick(float DeltaTime) override;

    // Functionality for picking up parcels.
    void PickUp(const AThirdPersonCharacter* Character);

    // Functionality for throwing parcels.
    void Throw(FVector TargetLocation, const AThirdPersonCharacter* Character);

    // Starts the movement of the parcel towards a target location.
    void StartMoveToTarget(FVector TargetLocation);

protected:
    virtual void BeginPlay() override;

private:
    bool bIsPickedUp;
    bool bMoving;
    bool bDelivered;
    bool bDestroyInitiated; // Flag to check if destroy process has started
    FVector MoveToLocation;
    float MoveSpeed;
    float MoveStartTime;
    float MoveDuration;

    // Timer handle for delayed destruction
    FTimerHandle DestroyTimerHandle;

    // Function to handle delayed destruction
    void DestroyParcel();
};