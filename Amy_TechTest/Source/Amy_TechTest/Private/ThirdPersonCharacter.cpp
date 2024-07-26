#include "ThirdPersonCharacter.h"
#include "Parcel.h"
#include "GameFramework/PlayerController.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Engine/World.h"
#include <Kismet/KismetSystemLibrary.h>
#include "House.h"
#include <Kismet/GameplayStatics.h>

// Add a variable to hold the target house
AHouse* TargetHouse = nullptr;

AThirdPersonCharacter::AThirdPersonCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    // Set size for collision capsule
    GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

    // Configure character movement
    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f);
    GetCharacterMovement()->JumpZVelocity = 600.f;
    GetCharacterMovement()->AirControl = 0.01f;

    // Movement parameters for a "slippery" feel similar to parcel run
    GroundFriction = 2.0f;
    BrakingDecelerationWalking = 800.0f;
    Acceleration = 2000.0f;

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

    HeldParcel = nullptr;
    ForwardValue = 0.0f;
    RightValue = 0.0f;

    // Initialize jump variables
    JumpCounter = 0;
    bCanDoubleJump = false;
    DashStrength = 1000.f;
}

void AThirdPersonCharacter::BeginPlay()
{
    Super::BeginPlay();
}

void AThirdPersonCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &AThirdPersonCharacter::Interact);
    PlayerInputComponent->BindAction("ThrowParcel", IE_Pressed, this, &AThirdPersonCharacter::ThrowParcel);

    PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AThirdPersonCharacter::Jump);
    PlayerInputComponent->BindAction("Jump", IE_Released, this, &AThirdPersonCharacter::StopJumping);

    // Bind movement events
    PlayerInputComponent->BindAxis("MoveForward", this, &AThirdPersonCharacter::MoveForward);
    PlayerInputComponent->BindAxis("MoveRight", this, &AThirdPersonCharacter::MoveRight);
}

void AThirdPersonCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // Update character rotation based on movement input
    UpdateCharacterRotation();
}

void AThirdPersonCharacter::MoveForward(float Value)
{
    ForwardValue = Value;
    if ((Controller != nullptr) && (Value != 0.0f))
    {
        // Find out which way is forward
        const FRotator Rotation = Controller->GetControlRotation();
        const FRotator YawRotation(0, Rotation.Yaw, 0);

        // Get forward vector
        const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
        AddMovementInput(Direction, Value);
    }
}

void AThirdPersonCharacter::MoveRight(float Value)
{
    RightValue = Value;
    if ((Controller != nullptr) && (Value != 0.0f))
    {
        // Find out which way is right
        const FRotator Rotation = Controller->GetControlRotation();
        const FRotator YawRotation(0, Rotation.Yaw, 0);

        // Get right vector
        const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
        AddMovementInput(Direction, Value);
    }
}

void AThirdPersonCharacter::UpdateCharacterRotation()
{
    if ((ForwardValue != 0.0f) || (RightValue != 0.0f))
    {
        const FRotator Rotation = Controller->GetControlRotation();
        const FRotator YawRotation(0, Rotation.Yaw, 0);

        // Combine movement direction
        FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X) * ForwardValue +
            FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y) * RightValue;

        // Update character rotation to face the direction of movement
        if (!Direction.IsNearlyZero())
        {
            FRotator NewRotation = Direction.Rotation();
            NewRotation.Pitch = 0; // Ensure the character doesn't pitch up/down
            NewRotation.Roll = 0;  // Ensure the character doesn't roll
            SetActorRotation(NewRotation);
        }
    }
}

void AThirdPersonCharacter::Jump()
{
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Jump Pressed"));
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
    TArray<AActor*> FoundHouses;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AHouse::StaticClass(), FoundHouses);
    if (FoundHouses.Num() > 0)
    {
        int Index = FMath::RandRange(0, FoundHouses.Num() - 1);
        TargetHouse = Cast<AHouse>(FoundHouses[Index]);

        // Debug log
        if (TargetHouse)
        {
            FVector Location = TargetHouse->GetActorLocation();
            float Radius = 300.0f;
            FColor Color = FColor::Emerald;
            float Duration = 5.0f;
            float Thickness = 5.0f;

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

        HeldParcel = nullptr;
        TargetHouse = nullptr;

        UE_LOG(LogTemp, Warning, TEXT("Parcel delivered and target house cleared."));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("No parcel to deliver or no target house."));
    }
}

void AThirdPersonCharacter::Interact()
{
    UE_LOG(LogTemp, Warning, TEXT("Interact pressed"));

    if (HeldParcel == nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("No parcel held, attempting to find nearest parcel"));
        HeldParcel = FindNearestParcel();
        if (HeldParcel)
        {
            HeldParcel->PickUp(this);
            SelectRandomHouse();
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Parcel held, checking if at target house"));
        if (IsAtTargetHouse())
        {
            DeliverParcel();
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Not at target house"));
        }
    }
}

AParcel* AThirdPersonCharacter::FindNearestParcel()
{
    TArray<AActor*> FoundActors;
    float NearestDist = FLT_MAX;
    AParcel* NearestParcel = nullptr;
    FVector MyLocation = GetActorLocation();
    float SearchRadius = 500.0f; // Define your search radius here

    UWorld* World = GetWorld();
    if (World)
    {
        TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
        ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_PhysicsBody));
        ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic));
        ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldDynamic));

        UKismetSystemLibrary::SphereOverlapActors(World, MyLocation, SearchRadius, ObjectTypes, AParcel::StaticClass(), TArray<AActor*>(), FoundActors);

        // Draw a debug sphere at the player's location with the defined radius
        DrawDebugSphere(World, MyLocation, SearchRadius, 32, FColor::Cyan, false, 10.0f, (uint8)'\000', 1.5f);

        if (FoundActors.Num() == 0)
        {
            UE_LOG(LogTemp, Warning, TEXT("No parcels found within range."));
            if (GEngine)
            {
                GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("No parcels found within range."));
            }
        }

        for (AActor* Actor : FoundActors)
        {
            float Dist = (Actor->GetActorLocation() - MyLocation).Size();
            if (Dist < NearestDist)
            {
                NearestParcel = Cast<AParcel>(Actor);
                NearestDist = Dist;
            }
        }
    }

    return NearestParcel;
}

void AThirdPersonCharacter::ThrowParcel()
{
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("ThrowParcel Pressed"));
    }
    UE_LOG(LogTemp, Warning, TEXT("ThrowParcel Pressed"));

    if (IsAtTargetHouse())
    {
        if (HeldParcel && TargetHouse)  // Ensure both the parcel and the target house are valid
        {
            FVector TargetLocation = TargetHouse->GetActorLocation();  // Use the house's location as the target

            UE_LOG(LogTemp, Log, TEXT("Throwing parcel towards house at location: %s"), *TargetLocation.ToString());
            HeldParcel->Throw(TargetLocation, this);
            HeldParcel = nullptr;  // Clear the held parcel after throwing
            DeliverParcel();
        }
        else if (!TargetHouse)
        {
            UE_LOG(LogTemp, Error, TEXT("No target house selected to throw the parcel at."));
            if (GEngine)
            {
                GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("No target house selected to throw the parcel at."));
            }
        }        
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Not at target house."));
        if (GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Not at target house."));
        }
    }


}