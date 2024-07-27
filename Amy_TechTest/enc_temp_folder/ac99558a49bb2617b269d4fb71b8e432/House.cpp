#include "House.h"
#include "Components/TimelineComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Curves/CurveFloat.h"

AHouse::AHouse()
{
    PrimaryActorTick.bCanEverTick = true;

    // Create and initialize the timeline component
    BounceTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("BounceTimeline"));

    // Assign the bounce curve (ensure you have a curve asset available)
    static ConstructorHelpers::FObjectFinder<UCurveFloat> BounceCurveAsset(TEXT("/Game/PathToYourCurve/BounceCurve.BounceCurve"));
    if (BounceCurveAsset.Succeeded())
    {
        BounceCurve = BounceCurveAsset.Object;
    }
}

void AHouse::BeginPlay()
{
    Super::BeginPlay();

    if (BounceCurve)
    {
        // Bind the timeline update and finished functions
        FOnTimelineFloat ProgressFunction;
        ProgressFunction.BindUFunction(this, FName("HandleBounceProgress"));
        BounceTimeline->AddInterpFloat(BounceCurve, ProgressFunction);

        FOnTimelineEvent FinishedFunction;
        FinishedFunction.BindUFunction(this, FName("HandleBounceFinished"));
        BounceTimeline->SetTimelineFinishedFunc(FinishedFunction);
    }
}

void AHouse::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AHouse::PlayBounceAnimation()
{
    if (BounceTimeline && BounceCurve)
    {
        BounceTimeline->PlayFromStart();
    }
}

void AHouse::HandleBounceProgress(float Value)
{
    // Apply the scale based on the curve value
    FVector NewScale = FVector(Value, Value, Value);
    SetActorScale3D(NewScale);
}

void AHouse::HandleBounceFinished()
{
    // Reset the scale to the original value
    SetActorScale3D(FVector(1.0f));
}