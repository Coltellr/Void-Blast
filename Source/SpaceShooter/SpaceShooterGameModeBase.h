#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Blueprint/UserWidget.h"
#include "SpaceShooterGameModeBase.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnScoreChangedSignature, int32, NewScore);

UCLASS()
class SPACESHOOTER_API ASpaceShooterGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	ASpaceShooterGameModeBase();

	UFUNCTION(BlueprintCallable, Category = "Score")
	void AddScore(int32 Points);

	UFUNCTION(BlueprintPure, Category = "Score")
	int32 GetScore() const { return CurrentScore; }

	UFUNCTION(BlueprintCallable, Category = "Game")
	void GameOver();

	UPROPERTY(BlueprintAssignable, Category = "Score")
	FOnScoreChangedSignature OnScoreChanged;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UUserWidget> HUDWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UUserWidget> GameOverWidgetClass;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Score")
	int32 CurrentScore;

	UPROPERTY()
	UUserWidget* CurrentHUDWidget;
};