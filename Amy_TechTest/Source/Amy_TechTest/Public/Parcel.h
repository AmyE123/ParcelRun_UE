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

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parcel")
        bool bDelivered;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parcel")
        bool bIsPickedUp;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
        class UBoxComponent* CollisionComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
        class UStaticMeshComponent* ParcelMesh;

private:
    // Private properties 
    bool bMoving;
    bool bDestroyInitiated;
    FVector MoveToLocation;
    float MoveSpeed;
    float MoveStartTime;
    float MoveDuration;
    FTimerHandle DestroyTimerHandle;

public:
    // Public functions
    AParcel();

    virtual void Tick(float DeltaTime) override;

    void PickUp(const AThirdPersonCharacter* Character);
    void Throw(FVector TargetLocation, const AThirdPersonCharacter* Character);
    void StartMoveToTarget(FVector TargetLocation);

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