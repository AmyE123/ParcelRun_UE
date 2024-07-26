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
    // Private properties
    UPROPERTY(EditAnywhere, Category = "Parcel")
        TSubclassOf<AParcel> ParcelClass;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
        USceneComponent* ParcelSpawningPoint;

    FTimerHandle SpawnTimerHandle;

public:
    // Public functions
    // Sets default values for this actor's properties
    APostbox();

    // Called every frame
    virtual void Tick(float DeltaTime) override;

protected:
    // Protected functions
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

private:
    // Private functions
    void SpawnParcel();

    UFUNCTION()
        void OnParcelPickedUp();
};