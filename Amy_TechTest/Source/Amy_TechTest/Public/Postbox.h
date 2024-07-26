// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Parcel.h"
#include "Postbox.generated.h"

UCLASS()
class AMY_TECHTEST_API APostbox : public AActor
{
    GENERATED_BODY()

public:
    // Sets default values for this actor's properties
    APostbox();

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

public:
    // Called every frame
    virtual void Tick(float DeltaTime) override;

private:
    // Parcel class to spawn
    UPROPERTY(EditAnywhere, Category = "Parcel")
        TSubclassOf<AParcel> ParcelClass;

    // Spawn point for the parcel
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
        USceneComponent* ParcelSpawningPoint;

    // Handle for the spawn timer
    FTimerHandle SpawnTimerHandle;

    // Function to spawn the parcel
    void SpawnParcel();

    // Function to handle parcel pickup event
    UFUNCTION()
        void OnParcelPickedUp();
};