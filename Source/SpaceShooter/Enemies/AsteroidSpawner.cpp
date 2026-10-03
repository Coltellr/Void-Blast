#include "AsteroidSpawner.h"
#include "Asteroid.h"
#include "Engine/World.h"
#include "TimerManager.h"

AAsteroidSpawner::AAsteroidSpawner()
{
	PrimaryActorTick.bCanEverTick = false;

	MinSpawnDelay = 0.8f;
	MaxSpawnDelay = 2.0f;

	SpawnMinX = -800.0f;
	SpawnMaxX = 800.0f;
	SpawnMinY = -1200.0f;
	SpawnMaxY = 1200.0f;
	SpawnMargin = 150.0f;
}

void AAsteroidSpawner::BeginPlay()
{
	Super::BeginPlay();
	ScheduleNextSpawn();
}

void AAsteroidSpawner::ScheduleNextSpawn()
{
	float RandomDelay = FMath::RandRange(MinSpawnDelay, MaxSpawnDelay);
	GetWorldTimerManager().SetTimer(SpawnTimerHandle, this, &AAsteroidSpawner::SpawnAsteroid, RandomDelay, false);
}

void AAsteroidSpawner::SpawnAsteroid()
{
	if (AsteroidClasses.Num() > 0)
	{
		int32 RandomIndex = FMath::RandRange(0, AsteroidClasses.Num() - 1);
		TSubclassOf<AAsteroid> SelectedClass = AsteroidClasses[RandomIndex];

		if (SelectedClass)
		{
			FVector SpawnLocation = FVector::ZeroVector;
			int32 Side = FMath::RandRange(0, 2);

			if (Side == 0)
			{
				SpawnLocation = FVector(SpawnMaxX + SpawnMargin, FMath::RandRange(SpawnMinY, SpawnMaxY), 32.0f);
			}
			else if (Side == 1)
			{
				SpawnLocation = FVector(FMath::RandRange(SpawnMinX, SpawnMaxX), SpawnMinY - SpawnMargin, 32.0f);
			}
			else
			{
				SpawnLocation = FVector(FMath::RandRange(SpawnMinX, SpawnMaxX), SpawnMaxY + SpawnMargin, 32.0f);
			}

			FActorSpawnParameters SpawnParams;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

			GetWorld()->SpawnActor<AAsteroid>(SelectedClass, SpawnLocation, FRotator::ZeroRotator, SpawnParams);
		}
	}

	ScheduleNextSpawn();
}