#include "EnemyCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SphereComponent.h"
#include "DrawDebugHelpers.h"
#include "TimerManager.h"

AEnemyCharacter::AEnemyCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    ChaseRange = 1000.0f;
    AttackRange = 200.0f;
    MovementSpeed = 300.0f;
    ShootingCooldown = 2.0f;  // 2-second cooldown
    bCanShoot = true;

    ChaseSphere = CreateDefaultSubobject<USphereComponent>(TEXT("ChaseSphere"));
    ChaseSphere->SetupAttachment(RootComponent);
    ChaseSphere->InitSphereRadius(ChaseRange);

    AttackSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AttackSphere"));
    AttackSphere->SetupAttachment(RootComponent);
    AttackSphere->InitSphereRadius(AttackRange);
}

void AEnemyCharacter::BeginPlay()
{
    Super::BeginPlay();

    PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
}

void AEnemyCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (PlayerCharacter)
    {
        float DistanceToPlayer = FVector::Dist(GetActorLocation(), PlayerCharacter->GetActorLocation());

        if (DistanceToPlayer <= AttackRange)
        {
            StopMovement();
            if (bCanShoot)
            {
                AttackPlayer();
                bCanShoot = false;
                GetWorld()->GetTimerManager().SetTimer(ShootingCooldownTimerHandle, this, &AEnemyCharacter::ResetShootingCooldown, ShootingCooldown, false);
            }
        }
        else if (DistanceToPlayer <= ChaseRange)
        {
            ChasePlayer();
        }
    }
}

void AEnemyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
}

bool AEnemyCharacter::IsPlayerInRange(float Range) const
{
    if (PlayerCharacter)
    {
        float DistanceToPlayer = FVector::Dist(GetActorLocation(), PlayerCharacter->GetActorLocation());
        return DistanceToPlayer <= Range;
    }
    return false;
}

void AEnemyCharacter::ChasePlayer()
{
    if (PlayerCharacter)
    {
        FVector Direction = (PlayerCharacter->GetActorLocation() - GetActorLocation()).GetSafeNormal();
        AddMovementInput(Direction, 1.0f);
        FRotator NewRotation = Direction.Rotation();
        SetActorRotation(NewRotation);
    }
}

void AEnemyCharacter::AttackPlayer()
{
    if (PlayerCharacter)
    {
        FVector Direction = (PlayerCharacter->GetActorLocation() - GetActorLocation()).GetSafeNormal();
        FRotator NewRotation = Direction.Rotation();
        SetActorRotation(NewRotation);

        FVector Start = GetActorLocation();
        FVector End = PlayerCharacter->GetActorLocation();

        FHitResult HitResult;
        FCollisionQueryParams Params;
        Params.AddIgnoredActor(this);

        bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, Params);

        // Ray for visual representation of shooting
        DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 0.2f, 0, 0.5f);

        if (bHit)
        {
            // Log hit information
            if (HitResult.GetActor())
            {
                UE_LOG(LogTemp, Warning, TEXT("Hit: %s"), *HitResult.GetActor()->GetName());
            }
            else
            {
                UE_LOG(LogTemp, Warning, TEXT("Hit: Unknown actor"));
            }

            // Implement shooting logic, e.g., apply damage to the player
            // Apply damage or other effects here
        }
        else
        {
            // Log that there was no hit
            UE_LOG(LogTemp, Warning, TEXT("No hit detected."));
        }
    }
}

void AEnemyCharacter::StopMovement()
{
    GetCharacterMovement()->StopMovementImmediately();
}

void AEnemyCharacter::ResetShootingCooldown()
{
    bCanShoot = true;
}