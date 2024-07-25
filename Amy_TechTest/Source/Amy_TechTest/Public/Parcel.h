// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "Parcel.generated.h"

UCLASS()
class AMY_TECHTEST_API AParcel : public AStaticMeshActor
{
    GENERATED_BODY()

public:
    AParcel();

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;

    void PickUp();
    void Throw(FVector TargetLocation);

private:
    bool bIsPickedUp;
};
