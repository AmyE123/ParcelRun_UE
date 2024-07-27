#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnemyCharacter.generated.h"

UCLASS()
class AMY_TECHTEST_API AEnemyCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    // Public Properties
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Audio")
    USoundCue* ShootingSoundCue;

protected:
    // Protected Properties
    virtual void BeginPlay() override;

private:
    // Private Properties

    UPROPERTY(EditAnywhere, Category = "AI")
    float ChaseRange;

    UPROPERTY(EditAnywhere, Category = "AI")
    float AttackRange;

    UPROPERTY(EditAnywhere, Category = "AI")
    float MovementSpeed;

    UPROPERTY(VisibleAnywhere, Category = "AI")
    class USphereComponent* ChaseSphere;

    UPROPERTY(VisibleAnywhere, Category = "AI")
    class USphereComponent* AttackSphere;

    ACharacter* PlayerCharacter;

    // Timer handle for shooting cooldown
    FTimerHandle ShootingCooldownTimerHandle;

    // Cooldown duration between shots
    UPROPERTY(EditAnywhere, Category = "AI")
    float ShootingCooldown;

    // Is the enemy ready to shoot
    bool bCanShoot;

public:
    // Public Functions
    AEnemyCharacter();
    virtual void Tick(float DeltaTime) override;

    /// <summary>
    /// Binds functionality to input
    /// </summary>
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
    // Private Functions
    void ChasePlayer();
    void AttackPlayer();
    void StopMovement();
    bool IsPlayerInRange(float Range) const;
    void ResetShootingCooldown();
};