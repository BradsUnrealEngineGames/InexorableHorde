// Bradley Olson - GNU General Public License v3.0

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "IHPowerupActor.generated.h"

class ASCharacter;

UCLASS()
class INEXORABLEHORDE_API AIHPowerupActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AIHPowerupActor();

protected:

	/* Time between powerup ticks */
	UPROPERTY(EditDefaultsOnly, Category = "Powerups")
	float PowerupInterval;

	/* Total times we apply the powerup effect */
	UPROPERTY(EditDefaultsOnly, Category = "Powerups")
	int32 TotalNumberOfTicks;

	int32 TicksProcessed;

	FTimerHandle TimerHandle_PowerupTick;

	UFUNCTION()
	void OnTickPowerup();

	UPROPERTY(ReplicatedUsing = OnRep_PowerupActive)
	bool IsPowerupActive;

	UFUNCTION()
	void OnRep_PowerupActive();

	UFUNCTION(BlueprintImplementableEvent, Category = "Powerups")
	void OnPowerupStateChanged(bool NewIsActive);
public:

	UFUNCTION(BlueprintImplementableEvent, Category = "Powerups")
	void OnPowerupTicked();

	UFUNCTION(BlueprintImplementableEvent, Category = "Powerups")
	void OnActivated(ASCharacter* ActivatingCharacter);

	UFUNCTION(BlueprintImplementableEvent, Category = "Powerups")
	void OnExpired();

	void ActivatePowerup(ASCharacter* ActivatingCharacter);
};
