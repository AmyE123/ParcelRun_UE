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

    // Don't rotate when the controller rotates. Let that just affect the camera.
    bUseControllerRotationPitch = false;
    bUseControllerRotationYaw = false;
    bUseControllerRotationRoll = false;

    // Configure character movement
    GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input
    GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // Rotation rate
    GetCharacterMovement()->JumpZVelocity = 600.f;
    GetCharacterMovement()->AirControl = 0.01f;

    // Movement parameters for a "slippery" feel
    GroundFriction = 2.0f; // Adjust this value for slipperiness
    BrakingDecelerationWalking = 800.0f; // Adjust this for how quickly the character slows down
    Acceleration = 2000.0f; // Adjust this for how quickly the character speeds up

    GetCharacterMovement()->BrakingDecelerationWalking = BrakingDecelerationWalking;
    GetCharacterMovement()->GroundFriction = GroundFriction;
    GetCharacterMovement()->MaxWalkSpeed = 600.0f; // Default walking speed
    GetCharacterMovement()->MaxWalkSpeedCrouched = 300.0f; // Crouched speed
    GetCharacterMovement()->MaxAcceleration = Acceleration;

    // Create a camera boom (pulls in towards the player if there is a collision)
    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(RootComponent);
    CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character
    CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

    // Create a follow camera
    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
    FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

    HeldParcel = nullptr;
    ForwardValue = 0.0f;
    RightValue = 0.0f;

    // Initialize jump variables
    JumpCounter = 0;
    bCanDoubleJump = false;
    DashStrength = 1000.f; // Adjust this value as needed
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
            float Radius = 300.0f; // Adjust this value based on the scale of your houses
            FColor Color = FColor::Emerald; // Change color based on your preference
            float Duration = 5.0f; // How long to display the sphere, in seconds
            float Thickness = 5.0f; // Line thickness

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
    if (TargetHouse && FVector::Dist(GetActorLocation(), TargetHouse->GetActorLocation()) < 100.0f)  // 100 is an example proximity range
    {
        return true;
    }
    return false;
}

void AThirdPersonCharacter::DeliverParcel()
{
    if (HeldParcel && TargetHouse)
    {
        // Trigger some effect or notification of delivery
        HeldParcel = nullptr;  // Clear the held parcel
        TargetHouse = nullptr; // Clear the target house
        // Possibly reward the player here
    }
}

void AThirdPersonCharacter::Interact()
{
    if (HeldParcel == nullptr)
    {
        HeldParcel = FindNearestParcel();
        if (HeldParcel)
        {
            HeldParcel->PickUp(this);
            SelectRandomHouse();  // Function to select a random house
        }
    }
    else if (IsAtTargetHouse())  // Check if at target house
    {
        DeliverParcel();  // Handle parcel delivery
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

    if (HeldParcel && TargetHouse)  // Ensure both the parcel and the target house are valid
    {
        FVector TargetLocation = TargetHouse->GetActorLocation();  // Use the house's location as the target

        // Optionally, adjust the target location for accuracy or game mechanics, e.g., aiming at the doorstep or a specific part of the house
        // TargetLocation += FVector(0, 0, -50); // Example adjustment if needed

        UE_LOG(LogTemp, Log, TEXT("Throwing parcel towards house at location: %s"), *TargetLocation.ToString());
        HeldParcel->Throw(TargetLocation, this);
        HeldParcel = nullptr;  // Clear the held parcel after throwing
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