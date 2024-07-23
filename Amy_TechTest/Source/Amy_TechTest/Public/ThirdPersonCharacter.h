#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ThirdPersonCharacter.generated.h"

class AParcel;

UCLASS()
class AMY_TECHTEST_API AThirdPersonCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    AThirdPersonCharacter();

protected:
    virtual void BeginPlay() override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    virtual void Landed(const FHitResult& Hit) override;

public:
    virtual void Tick(float DeltaTime) override;

    void Interact();
    void ThrowParcel();

    // Movement functions
    void MoveForward(float Value);
    void MoveRight(float Value);

    virtual void Jump() override;
    virtual void StopJumping() override;

private:
    void UpdateCharacterRotation();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
        class USpringArmComponent* CameraBoom;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
        class UCameraComponent* FollowCamera;

    // Input tracking variables
    float ForwardValue;
    float RightValue;

    int JumpCounter;
    bool bCanDoubleJump;
    float DashStrength;

    // Movement parameters for a "slippery" feel
    float GroundFriction;
    float BrakingDecelerationWalking;
    float Acceleration;

    AParcel* HeldParcel;
};