#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "InputActionValue.h"
#include "PlayerShip.generated.h"

class UBoxComponent;
class UStaticMeshComponent;
class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
class AProjectile;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnLivesChanged, int32, CurrentLives, int32, MaxLives);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnFuelChanged, float, CurrentFuel, float, MaxFuel);

UCLASS()
class SPACESHOOTER_API APlayerShip : public APawn
{
    GENERATED_BODY()

public:
    APlayerShip();

protected:
    virtual void BeginPlay() override;

    UFUNCTION(BlueprintImplementableEvent, Category = "Visuals")
    void OnPlayShootAnim();

    UFUNCTION(BlueprintImplementableEvent, Category = "Visuals")
    void OnPlayHitAnim();

    UFUNCTION(BlueprintImplementableEvent, Category = "Visuals")
    void OnPlayDeathAnim();

    UFUNCTION(BlueprintImplementableEvent, Category = "Visuals")
    void OnPlayBoostAnim();

    UFUNCTION(BlueprintImplementableEvent, Category = "Visuals")
    void OnStopBoostAnim();

public: 
    virtual void Tick(float DeltaTime) override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UBoxComponent* CollisionBox;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UStaticMeshComponent* ShipMesh;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    USpringArmComponent* CameraBoom;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UCameraComponent* CameraComponent;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flight")
    float MoveSpeed;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flight|Boost")
    float BaseMoveSpeed;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flight|Boost")
    float BoostMultiplier;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flight|Boost")
    float MaxFuel;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Flight|Boost")
    float CurrentFuel;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flight|Boost")
    float FuelConsumeDuration;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flight|Boost")
    float FuelRechargeDuration;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flight")
    float MinX;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flight")
    float MaxX;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flight")
    float MinY;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flight")
    float MaxY;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
    int32 MaxLives;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health")
    int32 CurrentLives;
    
    UPROPERTY(BlueprintAssignable, Category = "Health")
    FOnLivesChanged OnLivesChanged;

    UPROPERTY(BlueprintAssignable, Category = "Flight|Boost")
    FOnFuelChanged OnFuelChanged;

    UFUNCTION(BlueprintPure, Category = "Health")
    int32 GetCurrentLives() const { return CurrentLives; }

    UFUNCTION(BlueprintPure, Category = "Health")
    int32 GetMaxLives() const { return MaxLives; }
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enhanced Input")
    UInputMappingContext* DefaultMappingContext;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enhanced Input")
    UInputAction* MoveAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enhanced Input")
    UInputAction* FireAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Enhanced Input")
    UInputAction* BoostAction;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat")
    TSubclassOf<AProjectile> ProjectileClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    FVector MuzzleOffset;

private:
    FVector2D MovementInput;
    bool bIsBoosting;

    void Move(const FInputActionValue& Value);
    void Fire();
    void Die();
    void StartBoost();
    void StopBoost();
};