#include "ThirdPersonCharacter.h"
#include "Parcel.h"
#include "House.h"
#include "EnemyCharacter.h"
#include "DrawDebugHelpers.h"

#include "Components/CapsuleComponent.h"
#include "Sound/SoundCue.h"
#include "Camera/CameraComponent.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"

// Define static const variables
const float AThirdPersonCharacter::DashStrength = 1500.f;
const float AThirdPersonCharacter::GroundFriction = 2.0f;
const float AThirdPersonCharacter::BrakingDecelerationWalking = 800.0f;
const float AThirdPersonCharacter::Acceleration = 2000.0f;

AThirdPersonCharacter::AThirdPersonCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    // Set size for collision capsule
    GetCapsuleComponent()->InitCapsuleSize(42.0f, 96.0f);

    // Configure character movement
    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
    GetCharacterMovement()->JumpZVelocity = 600.0f;
    GetCharacterMovement()->AirControl = 0.01f;

    GetCharacterMovement()->BrakingDecelerationWalking = BrakingDecelerationWalking;
    GetCharacterMovement()->GroundFriction = GroundFriction;
    GetCharacterMovement()->MaxWalkSpeed = 600.0f;
    GetCharacterMovement()->MaxAcceleration = Acceleration;

    // Create a camera boom (pulls in towards the player if there is a collision)
    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(RootComponent);
    CameraBoom->TargetArmLength = 300.0f;
    CameraBoom->bUsePawnControlRotation = true;

    // Create a follow camera
    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
    FollowCamera->bUsePawnControlRotation = false;

    // Initialize movement variables
    ForwardValue = 0.0f;
    RightValue = 0.0f;

    // Initialize jump variables
    JumpCounter = 0;
    bCanDoubleJump = false;

    // Initialize parcel variables
    HeldParcel = nullptr;

    // Initialize house variables
    TargetHouse = nullptr;
    PreviousTargetHouse = nullptr;

    // Initialize delivered parcels counter
    DeliveredParcels = 0;

    Health = 3;
}

void AThirdPersonCharacter::BeginPlay()
{
    Super::BeginPlay();

    TArray<AActor*> FoundActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AHouse::StaticClass(), FoundActors);

    AllHouses.Empty();
    for (AActor* Actor : FoundActors)
    {
        AHouse* House = Cast<AHouse>(Actor);
        if (House)
        {
            AllHouses.Add(House);
        }
    }
}

void AThirdPersonCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    // Player movement functionality binding
    PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AThirdPersonCharacter::Jump);
    PlayerInputComponent->BindAction("Jump", IE_Released, this, &AThirdPersonCharacter::StopJumping);
    PlayerInputComponent->BindAxis("MoveForward", this, &AThirdPersonCharacter::MoveForward);
    PlayerInputComponent->BindAxis("MoveRight", this, &AThirdPersonCharacter::MoveRight);

    // Player input functionailty binding
    PlayerInputComponent->BindAction("ThrowParcel", IE_Pressed, this, &AThirdPersonCharacter::ThrowParcel);
}

void AThirdPersonCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    UpdateCharacterRotation();
    DrawArrowToTarget();
}

void AThirdPersonCharacter::MoveForward(float Value)
{
    ForwardValue = Value;
    if ((Controller != nullptr) && (Value != 0.0f))
    {
        FRotator Rotation = Controller->GetControlRotation();
        FRotator YawRotation(0, Rotation.Yaw, 0);

        FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
        AddMovementInput(Direction, Value);
    }
}

void AThirdPersonCharacter::MoveRight(float Value)
{
    RightValue = Value;
    if ((Controller != nullptr) && (Value != 0.0f))
    {
        FRotator Rotation = Controller->GetControlRotation();
        FRotator YawRotation(0, Rotation.Yaw, 0);

        FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
        AddMovementInput(Direction, Value);
    }
}

void AThirdPersonCharacter::UpdateCharacterRotation()
{
    if ((ForwardValue != 0.0f) || (RightValue != 0.0f))
    {
        FRotator Rotation = Controller->GetControlRotation();
        FRotator YawRotation(0, Rotation.Yaw, 0);

        FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X) * ForwardValue +
            FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y) * RightValue;

        if (!Direction.IsNearlyZero())
        {
            FRotator NewRotation = Direction.Rotation();
            NewRotation.Pitch = 0;
            NewRotation.Roll = 0;
            SetActorRotation(NewRotation);
        }
    }
}

void AThirdPersonCharacter::Jump()
{
    if (JumpCounter < 1)
    {
        ACharacter::Jump();
        JumpCounter++;
    }
    else if (bCanDoubleJump)
    {
        if (DoubleJumpSoundCue)
        {
            UGameplayStatics::PlaySoundAtLocation(this, DoubleJumpSoundCue, GetActorLocation());
        }

        FVector LaunchVelocity = GetActorForwardVector() * DashStrength;
        LaunchCharacter(LaunchVelocity, true, true);
        bCanDoubleJump = false;
        JumpCounter++;
    }
}

void AThirdPersonCharacter::StopJumping()
{
    Super::StopJumping();
}

void AThirdPersonCharacter::Landed(const FHitResult& Hit)
{
    Super::Landed(Hit);
    JumpCounter = 0;
    bCanDoubleJump = true;
}

void AThirdPersonCharacter::SelectRandomHouse()
{
    if (AllHouses.Num() > 0)
    {
        int32 Index;
        do
        {
            Index = FMath::RandRange(0, AllHouses.Num() - 1);
            TargetHouse = Cast<AHouse>(AllHouses[Index]);
        } while (TargetHouse == PreviousTargetHouse && AllHouses.Num() > 1);

        PreviousTargetHouse = TargetHouse;

        //if (TargetHouse)
        //{
        //    FVector Location = TargetHouse->GetActorLocation();
        //    const float Radius = 400.0f;
        //    const FColor Color = FColor::Emerald;
        //    const float Duration = 0.1f;
        //    const float Thickness = 1.0f;

        //    DrawDebugSphere(GetWorld(), Location, Radius, 32, Color, true, Duration, 0, Thickness);
        //}
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("No houses found to select from."));
    }
}

bool AThirdPersonCharacter::IsAtTargetHouse()
{
    if (TargetHouse)
    {
        FVector CharacterLocation = GetActorLocation();
        FVector HouseLocation = TargetHouse->GetActorLocation();
        float Distance = FVector::Dist(CharacterLocation, HouseLocation);

        // Player is at the target house and close enough to deliver.
        if (Distance < 300.0f)
        {
            return true;
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("No target house set."));
    }
    return false;
}

void AThirdPersonCharacter::DeliverParcel()
{
    if (HeldParcel && TargetHouse)
    {
        if (ThrowParcelSoundCue)
        {
            UGameplayStatics::PlaySoundAtLocation(this, ThrowParcelSoundCue, GetActorLocation());
        }

        TargetHouse->PlayBounceAnimation();
        HeldParcel = nullptr;
        TargetHouse = nullptr;
        UE_LOG(LogTemp, Warning, TEXT("Parcel delivered."));

        DeliveredParcels++;
        if (DeliveredParcels % 3 == 0)
        {
            SpawnEnemies();
        }
    }
}

void AThirdPersonCharacter::ThrowParcel()
{
    if (IsAtTargetHouse())
    {
        if (HeldParcel && TargetHouse)
        {
            FVector TargetLocation = TargetHouse->GetActorLocation();
            HeldParcel->Throw(TargetLocation, this);
            HeldParcel->bIsPickedUp = false;
            DeliverParcel();
        }
    }
}

void AThirdPersonCharacter::DrawArrowToTarget()
{
    if (TargetHouse)
    {
        FVector PlayerLocation = GetActorLocation();
        FVector TargetLocation = TargetHouse->GetActorLocation();
        FVector Direction = (TargetLocation - PlayerLocation).GetSafeNormal();

        DrawDebugDirectionalArrow(
            GetWorld(),
            PlayerLocation,
            PlayerLocation + Direction * 100.0f,
            100.0f,
            FColor::Green,
            false,
            -1.0f,
            0,
            5.0f
        );

        float Distance = FVector::Dist(PlayerLocation, TargetLocation);

        // Check if the distance is within 3 meters (300 units)
        FString DistanceString;
        if (Distance <= 300.0f)
        {
            // Displaying 0 just makes it easier for the player to tell they're at the house.
            DistanceString = TEXT("0 meters");
        }
        else
        {
            DistanceString = FString::Printf(TEXT("%.1f meters"), Distance / 100.0f);
        }

        // Draw the distance as text
        DrawDebugString(
            GetWorld(),
            PlayerLocation + FVector(0, 0, 100),
            DistanceString,
            nullptr,
            FColor::White,
            0.0f,
            true
        );
    }
}

void AThirdPersonCharacter::PlayerTakeDamage()
{
    Health -= 1;
}

void AThirdPersonCharacter::SpawnEnemies()
{
    if (EnemyClass)
    {
        UWorld* World = GetWorld();
        if (World)
        {
            for (int32 i = 0; i < EnemiesPerWave; i++)
            {
                FActorSpawnParameters SpawnParams;
                SpawnParams.Owner = this;
                SpawnParams.Instigator = GetInstigator();

                // Randomize spawn location around the designated SpawnLocation
                FVector SpawnLocationOffset = FVector(FMath::RandRange(-500, 500), FMath::RandRange(-500, 500), 0);
                FVector FinalSpawnLocation = SpawnLocation + SpawnLocationOffset;

                // Ensure the enemies spawn above ground level
                FVector GroundLocation = FinalSpawnLocation;
                GroundLocation.Z += 100.0f;

                AEnemyCharacter* SpawnedEnemy = World->SpawnActor<AEnemyCharacter>(EnemyClass, GroundLocation, FRotator::ZeroRotator, SpawnParams);
                if (SpawnedEnemy)
                {
                    // Ensure the AI controller is assigned
                    SpawnedEnemy->SpawnDefaultController();
                }
            }
        }
    }
}