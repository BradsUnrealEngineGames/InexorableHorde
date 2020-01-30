// Bradley Olson - GNU General Public License v3.0

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "IHGameMode.generated.h"

/**
 * 
 */
UCLASS()
class INEXORABLEHORDE_API AIHGameMode : public AGameModeBase
{
	GENERATED_BODY()

protected:

	//     Variables     //

	FTimerHandle TimerHandle_BotSpawner;

	FTimerHandle TimerHandle_NextWaveStart;

	int NumberOfBotsToSpawn;

	int WaveCount;

	UPROPERTY(EditDefaultsOnly, Category = "GameMode")
	float TimeBetweenWaves;

	//     Functions     //

	UFUNCTION(BlueprintImplementableEvent, Category = "GameMode")
	void SpawnNewBot();

	void SpawnBotTimerElapsed();
	
	void StartWave();

	void EndWave();

	void PrepareForNextWave();

	void CheckWaveState();

public:

	virtual void StartPlay() override;

	AIHGameMode();

	virtual void Tick(float DeltaSeconds) override;
};
