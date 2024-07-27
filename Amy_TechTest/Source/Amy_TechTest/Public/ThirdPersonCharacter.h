#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "DrawDebugHelpers.h"
#include "ThirdPersonCharacter.generated.h"

class AParcel;
class AHouse;

UCLASS()
class AMY_TECHTEST_API AThirdPersonCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    // Public properties
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
    USoundCue* DoubleJumpSoundCue;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
    USoundCue* ThrowParcelSoundCue;

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

    // Constant parameters for character movement
    static const float DashStrength;
    static const float GroundFriction;
    static const float BrakingDecelerationWalking;
    static const float Acceleration;

    // House parameters
    AHouse* TargetHouse;
    TArray<AHouse*> AllHouses;
    AHouse* PreviousTargetHouse;

public:
    // Public functions
    AThirdPersonCharacter();

    virtual void Tick(float DeltaTime) override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    void ThrowParcel();
    void SelectRandomHouse();

protected:
    // Protected functions
    virtual void BeginPlay() override;

private:
    // Private functions
    void MoveForward(float Value);
    void MoveRight(float Value);
    void UpdateCharacterRotation();
    void Jump() override;
    void StopJumping() override;
    void Landed(const FHitResult& Hit) override;    
    bool IsAtTargetHouse();
    void DeliverParcel();
    void DrawArrowToTarget();
};