#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ThirdPersonCharacter.generated.h"

class AParcel;
class AHouse;

UCLASS()
class AMY_TECHTEST_API AThirdPersonCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    AThirdPersonCharacter();

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
        class USpringArmComponent* CameraBoom;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
        class UCameraComponent* FollowCamera;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
        AParcel* HeldParcel;

    void Interact();
    void ThrowParcel();

private:
    void MoveForward(float Value);
    void MoveRight(float Value);
    void UpdateCharacterRotation();
    void Jump() override;
    void StopJumping() override;
    void Landed(const FHitResult& Hit) override;

    void SelectRandomHouse();
    bool IsAtTargetHouse();
    void DeliverParcel();
    AParcel* FindNearestParcel();

    // Movement properties
    float ForwardValue;
    float RightValue;
    int JumpCounter;
    bool bCanDoubleJump;
    float DashStrength;
    float GroundFriction;
    float BrakingDecelerationWalking;
    float Acceleration;

    // Target house for delivery
    AHouse* TargetHouse;

    // List of all houses and previous target house
    TArray<AHouse*> AllHouses;
    AHouse* PreviousTargetHouse;
};