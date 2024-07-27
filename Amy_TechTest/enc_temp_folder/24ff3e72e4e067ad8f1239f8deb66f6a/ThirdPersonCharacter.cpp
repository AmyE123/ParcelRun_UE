#include "ThirdPersonCharacter.h"
#include "Parcel.h"
#include "House.h"
#include "DrawDebugHelpers.h"

#include "Components/CapsuleComponent.h"
#include "Camera/CameraComponent.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"

// Define static const variables
const float AThirdPersonCharacter::DashStrength = 1000.f;
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
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Jump Pressed"));
    }
    UE_LOG(LogTemp, Warning, TEXT("Jump Pressed"));

    if (JumpCounter < 1)
    {
        ACharacter::Jump();
        JumpCounter++;
    }
    else if (bCanDoubleJump)
    {
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

        if (TargetHouse)
        {
            FVector Location = TargetHouse->GetActorLocation();
            const float Radius = 300.0f;
            const FColor Color = FColor::Emerald;
            const float Duration = 0.5f;
            const float Thickness = 0.5f;

            UE_LOG(LogTemp, Warning, TEXT("Selected House: %s at %s"), *TargetHouse->GetName(), *Location.ToString());
            DrawDebugSphere(GetWorld(), Location, Radius, 32, Color, true, Duration, 0, Thickness);

            UE_LOG(LogTemp, Warning, TEXT("Debug sphere should be visible at location: %s"), *Location.ToString());
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Failed to cast selected target as House."));
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("No houses found to select from."));
    }
}

bool AThirdPersonCharacter::IsAtTargetHouse()
{
    if (TargetHouse)
    {
        FVector CharacterLocation = GetActorLocation();
        FVector HouseLocation = TargetHouse->GetActorLocation();
        float Distance = FVector::Dist(CharacterLocation, HouseLocation);

        UE_LOG(LogTemp, Warning, TEXT("Character Location: %s"), *CharacterLocation.ToString());
        UE_LOG(LogTemp, Warning, TEXT("Target House Location: %s"), *HouseLocation.ToString());
        UE_LOG(LogTemp, Warning, TEXT("Distance to Target House: %f"), Distance);

        if (Distance < 300.0f)
        {
            UE_LOG(LogTemp, Warning, TEXT("Character is at the target house."));
            return true;
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Character is NOT at the target house."));
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("No target house set."));
    }
    return false;
}

void AThirdPersonCharacter::DeliverParcel()
{
    if (HeldParcel && TargetHouse)
    {
        UE_LOG(LogTemp, Warning, TEXT("Delivering parcel to house: %s"), *TargetHouse->GetName());
        TargetHouse->PlayBounceAnimation();
        HeldParcel = nullptr;
        TargetHouse = nullptr;
        UE_LOG(LogTemp, Warning, TEXT("Parcel delivered and target house cleared."));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("No parcel to deliver or no target house."));
    }
}

AParcel* AThirdPersonCharacter::FindNearestParcel()
{
    TArray<AActor*> FoundActors;
    const float SearchRadius = 200.0f;
    FVector MyLocation = GetActorLocation();
    float NearestDist = FLT_MAX;
    AParcel* NearestParcel = nullptr;

    UWorld* World = GetWorld();
    if (World)
    {
        TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
        ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_PhysicsBody));
        ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic));
        ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldDynamic));

        UKismetSystemLibrary::SphereOverlapActors(World, MyLocation, SearchRadius, ObjectTypes, AParcel::StaticClass(), TArray<AActor*>(), FoundActors);
        DrawDebugSphere(World, MyLocation, SearchRadius, 32, FColor::Cyan, false, 10.0f, (uint8)'\000', 1.5f);

        if (FoundActors.Num() == 0)
        {
            UE_LOG(LogTemp, Warning, TEXT("No parcels found within range."));
            if (GEngine)
            {
                GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("No parcels found within range."));
            }
        }

        for (AActor* Actor : FoundActors)
        {
            AParcel* Parcel = Cast<AParcel>(Actor);
            if (Parcel && !Parcel->bDelivered && !Parcel->bIsPickedUp)
            {
                float Dist = (Parcel->GetActorLocation() - MyLocation).Size();
                if (Dist < NearestDist)
                {
                    NearestParcel = Parcel;
                    NearestDist = Dist;
                }
            }
        }
    }

    return NearestParcel;
}

void AThirdPersonCharacter::ThrowParcel()
{
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("ThrowParcel Pressed"));
    }
    UE_LOG(LogTemp, Warning, TEXT("ThrowParcel Pressed"));

    if (IsAtTargetHouse())
    {
        if (HeldParcel && TargetHouse)
        {
            FVector TargetLocation = TargetHouse->GetActorLocation();
            UE_LOG(LogTemp, Log, TEXT("Throwing parcel towards house at location: %s"), *TargetLocation.ToString());
            HeldParcel->Throw(TargetLocation, this);
            HeldParcel->bIsPickedUp = false; // Mark the parcel as not held
            DeliverParcel();
        }
        else if (!TargetHouse)
        {
            UE_LOG(LogTemp, Error, TEXT("No target house selected to throw the parcel at."));
            if (GEngine)
            {
                GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("No target house selected to throw the parcel at."));
            }
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Not at target house."));
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Not at target house."));
        }
    }
}