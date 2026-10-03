#include "Asteroid.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/DamageEvents.h"
#include "../SpaceShooterGameModeBase.h"
#include "../Player/PlayerShip.h"

AAsteroid::AAsteroid()
{
    PrimaryActorTick.bCanEverTick = true;

    CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionSphere"));
    CollisionSphere->InitSphereRadius(40.0f);
    CollisionSphere->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
    RootComponent = CollisionSphere;

    AsteroidMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AsteroidMesh"));
    AsteroidMesh->SetupAttachment(RootComponent);
    AsteroidMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    MinHitsToDestroy = 1;
    MaxHitsToDestroy = 3;
    CollisionDamage = 1.0f;
    MoveSpeed = 400.0f;
    ScoreValue = 100;
    InitialLifeSpan = 15.0f;
    RotationSpeed = 45.0f;
    bIsDying = false;
}

void AAsteroid::BeginPlay()
{
    Super::BeginPlay();

    CollisionSphere->OnComponentBeginOverlap.AddDynamic(this, &AAsteroid::OnOverlap);

    CurrentHitsLeft = FMath::RandRange(MinHitsToDestroy, MaxHitsToDestroy);
    RotationSpeed = FMath::RandRange(-60.0f, 60.0f);

    APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
    if (PlayerPawn)
    {
        FVector DirToPlayer = (PlayerPawn->GetActorLocation() - GetActorLocation()).GetSafeNormal();
        MoveDirection = FVector(DirToPlayer.X, DirToPlayer.Y, 0.0f);
    }
    else
    {
        MoveDirection = FVector(0.0f, -1.0f, 0.0f);
    }
}

void AAsteroid::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (!bIsDying)
    {
        FVector NewLocation = GetActorLocation() + (MoveDirection * MoveSpeed * DeltaTime);
        SetActorLocation(NewLocation, true);

        AddActorLocalRotation(FRotator(0.0f, RotationSpeed * DeltaTime, 0.0f));
    }
}

void AAsteroid::TakeProjectileHit()
{
    if (bIsDying)
    {
        return;
    }

    CurrentHitsLeft--;
    if (CurrentHitsLeft <= 0)
    {
        if (ASpaceShooterGameModeBase* GM = Cast<ASpaceShooterGameModeBase>(UGameplayStatics::GetGameMode(this)))
        {
            GM->AddScore(ScoreValue);
        }
        Die();
    }
}

void AAsteroid::Die()
{
    bIsDying = true;
    SetActorEnableCollision(false);
    OnPlayDeathAnim();
}

void AAsteroid::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (!OtherActor || OtherActor == this || bIsDying)
    {
        return;
    }

    if (APlayerShip* Player = Cast<APlayerShip>(OtherActor))
    {
        UGameplayStatics::ApplyDamage(Player, CollisionDamage, nullptr, this, UDamageType::StaticClass());
        Die();
    }
}