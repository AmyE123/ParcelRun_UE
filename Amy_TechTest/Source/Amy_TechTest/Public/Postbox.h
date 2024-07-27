#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Parcel.h"
#include "Postbox.generated.h"

UCLASS()
class AMY_TECHTEST_API APostbox : public AActor
{
    GENERATED_BODY()

private:
    // private properties
    UPROPERTY(EditAnywhere, Category = "Parcel")
    TSubclassOf<AParcel> ParcelClass;

    UPROPERTY(EditAnywhere, Category = "Parcel")
    float ParcelRespawnRate;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
    USceneComponent* ParcelSpawningPoint;

    FTimerHandle SpawnTimerHandle;

public:
    // Public functions
    APostbox();
    virtual void Tick(float DeltaTime) override;

protected:
    // Protected functions
    virtual void BeginPlay() override;

private:
    // Private functions
    void SpawnParcel();

    UFUNCTION()
        void OnParcelPickedUp();


};