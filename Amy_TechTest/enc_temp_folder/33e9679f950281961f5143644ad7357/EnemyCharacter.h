#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnemyCharacter.generated.h"

UCLASS()
class AMY_TECHTEST_API AEnemyCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    AEnemyCharacter();

protected:
    virtual void BeginPlay() override;

public:
    virtual void Tick(float DeltaTime) override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
    void ChasePlayer();
    void AttackPlayer();
    void StopMovement();
    bool IsPlayerInRange(float Range) const;
    void ResetShootingCooldown();

    UPROPERTY(EditAnywhere, Category = "AI")
        float ChaseRange;

    UPROPERTY(EditAnywhere, Category = "AI")
        float AttackRange;

    UPROPERTY(EditAnywhere, Category = "AI")
        float MovementSpeed;

    UPROPERTY(EditAnywhere, Category = "AI")
        float ShootingCooldown;

    UPROPERTY(EditAnywhere, Category = "AI")
        class USphereComponent* ChaseSphere;

    UPROPERTY(EditAnywhere, Category = "AI")
        class USphereComponent* AttackSphere;

    UPROPERTY(EditAnywhere, Category = "Audio")
        class USoundCue* ShootingSoundCue;

    FTimerHandle ShootingCooldownTimerHandle;
    bool bCanShoot;

    ACharacter* PlayerCharacter;
};