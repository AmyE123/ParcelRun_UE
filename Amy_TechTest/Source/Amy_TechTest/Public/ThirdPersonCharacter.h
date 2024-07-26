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

public:
    virtual void Tick(float DeltaTime) override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
    virtual void BeginPlay() override;

public:
    // Public properties
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
        class USpringArmComponent* CameraBoom;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
        class UCameraComponent* FollowCamera;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
        AParcel* HeldParcel;

private:
    // Private properties
    float ForwardValue;
    float RightValue;
    int JumpCounter;
    bool bCanDoubleJump;
    float DashStrength;
    float GroundFriction;
    float BrakingDecelerationWalking;
    float Acceleration;

    AHouse* TargetHouse;
    TArray<AHouse*> AllHouses;
    AHouse* PreviousTargetHouse;

public:
    // Public functions
    void Interact();
    void ThrowParcel();

private:
    // Private functions
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
};