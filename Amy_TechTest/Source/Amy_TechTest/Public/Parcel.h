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
    AParcel();

public:
    virtual void Tick(float DeltaTime) override;

protected:
    virtual void BeginPlay() override;

public:
    // Public properties
    FParcelPickedUpDelegate OnParcelPickedUp;

private:
    // Private properties
    bool bIsPickedUp;
    bool bMoving;
    bool bDelivered;
    bool bDestroyInitiated;
    FVector MoveToLocation;
    float MoveSpeed;
    float MoveStartTime;
    float MoveDuration;
    FTimerHandle DestroyTimerHandle;

public:
    // Public functions
    void PickUp(const AThirdPersonCharacter* Character);
    void Throw(FVector TargetLocation, const AThirdPersonCharacter* Character);
    void StartMoveToTarget(FVector TargetLocation);

private:
    // Private functions
    void DestroyParcel();
};