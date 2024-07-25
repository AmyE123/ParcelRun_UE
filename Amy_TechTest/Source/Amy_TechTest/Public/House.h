#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
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

    // Add more functions related to the house here
};