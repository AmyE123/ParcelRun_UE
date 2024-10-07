#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "ThirdPersonCharacter.h"
#include "GameFramework/Actor.h"
#include "Parcel.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FParcelPickedUpDelegate);

UCLASS()
class AMY_TECHTEST_API AParcel : public AActor
{
    GENERATED_BODY()

public:
    // Public properties
    FParcelPickedUpDelegate OnParcelPickedUp;

    /// <summary>
    /// Whether the parcel has been delivered to the house yet or not.
    /// </summary>
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parcel")
    bool bDelivered;

    /// <summary>
    /// Whether the parcel has been picked up from the post box yet or not.
    /// </summary>
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parcel")
    bool bIsPickedUp;

    /// <summary>
    /// The box collider for the parcel which is used for detecting overlap for pickup.
    /// </summary>
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    class UBoxComponent* CollisionComponent;

    /// <summary>
    /// The parcel mesh component.
    /// </summary>
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    class UStaticMeshComponent* ParcelMesh;

private:
    // Private properties 
    bool bMoving;
    bool bDestroyInitiated;
    FVector MoveToLocation;
    FTimerHandle DestroyTimerHandle;
    float MoveStartTime;
    float MoveDuration;

    // Static constants
    static const float MoveSpeed;

public:
    // Public functions
    AParcel();

    virtual void Tick(float DeltaTime) override;

    /// <summary>
    /// Functionality for picking up parcels.
    /// </summary>
    /// <param name="Character">The character which should pick up the parcel.</param>
    void PickUp(const AThirdPersonCharacter* Character);

    /// <summary>
    /// Functionality for throwing parcels.
    /// </summary>
    /// <param name="TargetLocation">The target location to throw the parcel to (Usually a house).</param>
    /// <param name="Character">The character which should throw the parcel.</param>
    void Throw(FVector TargetLocation, const AThirdPersonCharacter* Character);

    /// <summary>
    /// Functionality for the parcel to move to the target.
    /// </summary>
    /// <param name="TargetLocation">The target location to move the parcel to (Usually a house).</param>
    void StartMoveToTarget(FVector TargetLocation);

    /// <summary>
    /// Checks if the player character is overlapping a parcel, and runs the appropriate functionalities if they are.
    /// The only param used is OtherActor but it wont compile without the other params.
    /// </summary>
    UFUNCTION()
    void OnOverlapBegin(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor,
                        class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                        const FHitResult& SweepResult);

protected:
    // Protected functions
    virtual void BeginPlay() override;

private:
    // Private functions
    void DestroyParcel();
};