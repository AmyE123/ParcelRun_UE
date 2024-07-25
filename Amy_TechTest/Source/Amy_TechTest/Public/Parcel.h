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
    // Tracks whether the parcel had been picked up
    bool bIsPickedUp;

    // Indicates if the parcel is currently moving towards a target
    bool Moving;

    // The target location where the parcel should move to
    FVector MoveToLocation;

    // The speed at which the parcel moves towards the target
    float MoveSpeed;

    // The start time of the move
    float MoveStartTime;

    // The duration of the move from start to finish
    float MoveDuration;

    // Public Functions
public:
    // Parcel's constructor
    AParcel();

    // Tick function override from AActor
    virtual void Tick(float DeltaTime) override;

    // Functionality for picking up parcels
    void PickUp(const AThirdPersonCharacter* Character);

    // Functionality for throwing parcels
    void Throw(FVector TargetLocation, const AThirdPersonCharacter* Character);

    // Starts the movement of the parcel towards a target location
    void StartMoveToTarget(FVector TargetLocation);

    // Protected Functions    
protected:

    // Called when the game starts
    virtual void BeginPlay() override;

    // Private Functions
private:

};