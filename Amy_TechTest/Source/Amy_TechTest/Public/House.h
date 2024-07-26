#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/TimelineComponent.h"
#include "House.generated.h"

UCLASS()
class AMY_TECHTEST_API AHouse : public AActor
{
    GENERATED_BODY()

public:
    AHouse();

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;

    // Function to trigger the bounce animation
    void PlayBounceAnimation();

private:
    // Timeline component to handle the bounce animation
    UPROPERTY()
        UTimelineComponent* BounceTimeline;

    // Curve for the bounce animation
    UPROPERTY(EditAnywhere, Category = "Animation")
        UCurveFloat* BounceCurve;

    // Callback function for timeline updates
    UFUNCTION()
        void HandleBounceProgress(float Value);

    // Callback function for timeline finished
    UFUNCTION()
        void HandleBounceFinished();
};