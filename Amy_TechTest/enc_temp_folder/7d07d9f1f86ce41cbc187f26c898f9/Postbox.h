#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
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

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components")
        class USceneComponent* ParcelSpawningPoint;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parcel")
        TSubclassOf<class AParcel> ParcelClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Parcel")
        float ParcelRespawnRate;

private:
    FTimerHandle SpawnTimerHandle;

    void SpawnParcel();

    UFUNCTION()
        void OnParcelPickedUp();
};