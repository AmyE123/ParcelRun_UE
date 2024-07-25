// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "ThirdPersonCharacter.h"
#include "Parcel.generated.h"

UCLASS()
class AMY_TECHTEST_API AParcel : public AStaticMeshActor
{
    GENERATED_BODY()

// Public Properties
public:

// Protected Properties
protected:

// Private Properties
private:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parcel", meta = (AllowPrivateAccess = "true"))
    bool bIsPickedUp;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parcel", meta = (AllowPrivateAccess = "true"))
    bool bMoving;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parcel", meta = (AllowPrivateAccess = "true"))
    FVector MoveToLocation;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parcel", meta = (AllowPrivateAccess = "true"))
    float MoveSpeed;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parcel", meta = (AllowPrivateAccess = "true"))
    float MoveStartTime;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parcel", meta = (AllowPrivateAccess = "true"))
    float MoveDuration;

// Public Functions
public:
    /// <summary>
    /// The parcel's constructor.
    /// </summary>
    AParcel();

    /// <summary>
    /// Tick function override from AActor.
    /// </summary>
    virtual void Tick(float DeltaTime) override;

    /// <summary>
    /// Functionality for picking up parcels.
    /// </summary>
    /// <param name="Character">The player character which will pick up the parcel.</param>
    void PickUp(const AThirdPersonCharacter* Character);

    /// <summary>
    /// Functionality for throwing parcels.
    /// </summary>
    /// <param name="TargetLocation">The target location that the parcel needs to be thrown towards.</param>
    /// <param name="Character">The player character which will throw the parcel.</param>
    void Throw(FVector TargetLocation, const AThirdPersonCharacter* Character);

    /// <summary>
    /// Starts the movement of the parcel towards a target location.
    /// </summary>
    /// <param name="TargetLocation">The target location that the parcel will move towards.</param>
    void StartMoveToTarget(FVector TargetLocation);

// Protected Functions    
protected:
    virtual void BeginPlay() override;

// Private Functions
private:

};