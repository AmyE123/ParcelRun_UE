#include "EnemyCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SphereComponent.h"
#include "Sound/SoundCue.h"
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

    // Initialize the chase sphere
    ChaseSphere = CreateDefaultSubobject<USphereComponent>(TEXT("ChaseSphere"));
    ChaseSphere->SetupAttachment(RootComponent);
    ChaseSphere->InitSphereRadius(ChaseRange);

    // Initialize the attack sphere
    AttackSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AttackSphere"));
    AttackSphere->SetupAttachment(RootComponent);
    AttackSphere->InitSphereRadius(AttackRange);

    // Configure character movement
    GetCharacterMovement()->MaxWalkSpeed = MovementSpeed;
    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
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
        AddMovementInput(Direction);

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
        DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 0.2f, 0, 1.0f);

        if (ShootingSoundCue)
        {
            UGameplayStatics::PlaySoundAtLocation(this, ShootingSoundCue, GetActorLocation());
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