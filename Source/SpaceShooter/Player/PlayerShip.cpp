#include "PlayerShip.h"
#include "Projectile.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "../SpaceShooterGameModeBase.h"
#include "Kismet/GameplayStatics.h"

APlayerShip::APlayerShip()
{
    PrimaryActorTick.bCanEverTick = true;

    CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
    CollisionBox->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
    CollisionBox->SetGenerateOverlapEvents(true);
    RootComponent = CollisionBox;

    ShipMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShipMesh"));
    ShipMesh->SetupAttachment(RootComponent);

    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(RootComponent);
    CameraBoom->SetUsingAbsoluteRotation(true);
    CameraBoom->TargetArmLength = 1500.0f;
    CameraBoom->SetRelativeRotation(FRotator(-90.0f, 0.0f, 0.0f));
    CameraBoom->bDoCollisionTest = false;

    CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
    CameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
    CameraComponent->bUsePawnControlRotation = false;
    
    MaxLives = 3;
    CurrentLives = MaxLives;

    BaseMoveSpeed = 800.0f;
    MoveSpeed = BaseMoveSpeed;
    BoostMultiplier = 3.0f;
    MaxFuel = 100.0f;
    CurrentFuel = MaxFuel;
    FuelConsumeDuration = 3.0f;
    FuelRechargeDuration = 12.0f;
    bIsBoosting = false;
    
    MinX = -800.0f;
    MaxX = 800.0f;
    MinY = -1200.0f;
    MaxY = 1200.0f;
    MovementInput = FVector2D::ZeroVector;

    MuzzleOffset = FVector(100.0f, 0.0f, 0.0f);

    Tags.AddUnique(TEXT("Player"));
}

void APlayerShip::BeginPlay()
{
    Super::BeginPlay();

    CurrentLives = MaxLives;
    OnLivesChanged.Broadcast(CurrentLives, MaxLives);
    OnFuelChanged.Broadcast(CurrentFuel, MaxFuel);

    CameraBoom->SetUsingAbsoluteLocation(true);
    CameraBoom->SetWorldLocation(FVector(0.0f, 0.0f, 1500.0f));

    if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
        {
            if (DefaultMappingContext)
            {
                Subsystem->AddMappingContext(DefaultMappingContext, 0);
            }
        }
    }
}

void APlayerShip::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (bIsBoosting)
    {
        if (CurrentFuel > 0.0f)
        {
            float ConsumeRate = MaxFuel / FuelConsumeDuration;
            CurrentFuel -= ConsumeRate * DeltaTime;
            
            if (CurrentFuel <= 0.0f)
            {
                CurrentFuel = 0.0f;
                StopBoost();
            }
        }
    }
    else
    {
        if (CurrentFuel < MaxFuel)
        {
            float RechargeRate = MaxFuel / FuelRechargeDuration;
            CurrentFuel += RechargeRate * DeltaTime;
            
            if (CurrentFuel > MaxFuel)
            {
                CurrentFuel = MaxFuel;
            }
        }
    }

    OnFuelChanged.Broadcast(CurrentFuel, MaxFuel);

    if (!MovementInput.IsNearlyZero())
    {
        FVector MoveDirection = FVector(MovementInput.Y, MovementInput.X, 0.0f).GetSafeNormal();
        FVector NewLocation = GetActorLocation() + (MoveDirection * MoveSpeed * DeltaTime);

        NewLocation.X = FMath::Clamp(NewLocation.X, MinX, MaxX);
        NewLocation.Y = FMath::Clamp(NewLocation.Y, MinY, MaxY);

        SetActorLocation(NewLocation, true);

        MovementInput = FVector2D::ZeroVector;
    }
}

void APlayerShip::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
    {
        if (MoveAction)
        {
            EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlayerShip::Move);
        }

        if (FireAction)
        {
            EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &APlayerShip::Fire);
        }

        if (BoostAction)
        {
            EnhancedInputComponent->BindAction(BoostAction, ETriggerEvent::Started, this, &APlayerShip::StartBoost);
            EnhancedInputComponent->BindAction(BoostAction, ETriggerEvent::Completed, this, &APlayerShip::StopBoost);
            EnhancedInputComponent->BindAction(BoostAction, ETriggerEvent::Canceled, this, &APlayerShip::StopBoost);
        }
    }
}

void APlayerShip::Move(const FInputActionValue& Value)
{
    MovementInput = Value.Get<FVector2D>();
}

void APlayerShip::Fire()
{
    if (ProjectileClass)
    {
        FVector SpawnLocation = GetActorLocation() + (GetActorForwardVector() * MuzzleOffset.X) + (GetActorRightVector() * MuzzleOffset.Y) + (GetActorUpVector() * MuzzleOffset.Z);
        FRotator SpawnRotation = GetActorRotation();

        FActorSpawnParameters SpawnParams;
        SpawnParams.Owner = this;
        SpawnParams.Instigator = GetInstigator();

        GetWorld()->SpawnActor<AProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, SpawnParams);

        OnPlayShootAnim();
    }
}

float APlayerShip::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
    float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
    
    int32 DamageToInt = FMath::Max(1, FMath::RoundToInt(ActualDamage));
    CurrentLives = FMath::Clamp(CurrentLives - DamageToInt, 0, MaxLives);
    
    OnLivesChanged.Broadcast(CurrentLives, MaxLives);

    if (CurrentLives <= 0)
    {
        Die();
    }
    else
    {
        OnPlayHitAnim();
    }

    return ActualDamage;
}

void APlayerShip::Die()
{
    

    SetActorEnableCollision(false);

    if (APlayerController* PC = Cast<APlayerController>(GetController()))
    {
        DisableInput(PC);
    }

    MovementInput = FVector2D::ZeroVector;

    OnPlayDeathAnim();
}

void APlayerShip::StartBoost()
{
    if (CurrentFuel > 0.0f)
    {
        bIsBoosting = true;
        MoveSpeed = BaseMoveSpeed * BoostMultiplier;
        OnPlayBoostAnim();
    }
}

void APlayerShip::StopBoost()
{
    if (bIsBoosting)
    {
        bIsBoosting = false;
        MoveSpeed = BaseMoveSpeed;
        OnStopBoostAnim();
    }
}