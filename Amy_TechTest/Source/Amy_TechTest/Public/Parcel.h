// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
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

// Public Functions
public:
    // Parcel's constructor
    AParcel();

    // Tick function override from AActor
    virtual void Tick(float DeltaTime) override;

    // Functionality for picking up parcels
    void PickUp();

    // Functionality for throwing parcels
    void Throw(FVector TargetLocation);

// Protected Functions    
protected:
    
    // Called when the game starts
    virtual void BeginPlay() override;

// Private Functions
private:  
    
};
