#include "SpaceShooterGameModeBase.h"
#include "Kismet/GameplayStatics.h"

ASpaceShooterGameModeBase::ASpaceShooterGameModeBase()
{
	CurrentScore = 0;
}

void ASpaceShooterGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	if (HUDWidgetClass)
	{
		CurrentHUDWidget = CreateWidget<UUserWidget>(GetWorld(), HUDWidgetClass);
		if (CurrentHUDWidget)
		{
			CurrentHUDWidget->AddToViewport();
		}
	}

	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (PC)
	{
		PC->bShowMouseCursor = false;
		FInputModeGameOnly InputMode;
		PC->SetInputMode(InputMode);
	}
}

void ASpaceShooterGameModeBase::AddScore(int32 Points)
{
	CurrentScore += Points;
	OnScoreChanged.Broadcast(CurrentScore);
}

void ASpaceShooterGameModeBase::GameOver()
{
	if (CurrentHUDWidget)
	{
		CurrentHUDWidget->RemoveFromParent();
	}

	if (GameOverWidgetClass)
	{
		UUserWidget* GameOverWidget = CreateWidget<UUserWidget>(GetWorld(), GameOverWidgetClass);
		if (GameOverWidget)
		{
			GameOverWidget->AddToViewport();

			APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
			if (PC)
			{
				PC->bShowMouseCursor = true;
				FInputModeUIOnly InputMode;
				PC->SetInputMode(InputMode);
				UGameplayStatics::SetGamePaused(GetWorld(), true);
			}
		}
	}
}