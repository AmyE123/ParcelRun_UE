#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/TimelineComponent.h"
#include "House.generated.h"

UCLASS()
class AMY_TECHTEST_API AHouse : public AActor
{
    GENERATED_BODY()

private:
    // Private properties
    UPROPERTY()
    UTimelineComponent* BounceTimeline;

    UPROPERTY(EditAnywhere, Category = "Animation")
    UCurveFloat* BounceCurve;

public:
    // Public functions
    AHouse();

    virtual void Tick(float DeltaTime) override;

    /// <summary>
    /// Plays the bounce animation when the house recieves a parcel.
    /// </summary>
    void PlayBounceAnimation();

protected:
    // Protected functions
    virtual void BeginPlay() override;

private:
    // Private functions
    UFUNCTION()
    void HandleBounceProgress(float Value);

    UFUNCTION()
    void HandleBounceFinished();
};