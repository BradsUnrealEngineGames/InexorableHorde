// Bradley Olson - GNU General Public License v3.0


#include "IHGameMode.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "IHHealthComponent.h"

AIHGameMode::AIHGameMode()
{
	TimeBetweenWaves = 2.0f;

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 1.0f;
}

void AIHGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	CheckWaveState();
}

void AIHGameMode::StartPlay()
{
	Super::StartPlay();

	PrepareForNextWave();
}

void AIHGameMode::StartWave()
{
	WaveCount++;

	NumberOfBotsToSpawn = 2 * WaveCount;

	GetWorldTimerManager().SetTimer(TimerHandle_BotSpawner, this, &AIHGameMode::SpawnBotTimerElapsed, 1.0f, true, 0.0f);
}

void AIHGameMode::SpawnBotTimerElapsed()
{

	SpawnNewBot();

	NumberOfBotsToSpawn--;

	if (NumberOfBotsToSpawn <= 0)
	{
		EndWave();
	}
}

void AIHGameMode::EndWave()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_BotSpawner);
}

void AIHGameMode::PrepareForNextWave()
{
	GetWorldTimerManager().SetTimer(TimerHandle_NextWaveStart, this, &AIHGameMode::StartWave, TimeBetweenWaves, false);
}

void AIHGameMode::CheckWaveState()
{
	bool isPreparingForWave = GetWorldTimerManager().IsTimerActive(TimerHandle_NextWaveStart);

	if (NumberOfBotsToSpawn > 0 || isPreparingForWave) { return; }

	bool isAnyBotAlive = false;

	for (FConstPawnIterator It = GetWorld()->GetPawnIterator(); It; ++It)
	{
		APawn* TestPawn = It->Get();
		if (TestPawn == nullptr || TestPawn->IsPlayerControlled())
		{
			continue;
		}

		UIHHealthComponent* HealthComp = Cast<UIHHealthComponent>(TestPawn->GetComponentByClass(UIHHealthComponent::StaticClass()));
		if (HealthComp && HealthComp->GetHealth() > 0.0f)
		{
			isAnyBotAlive = true;
			break;
		}
	}
	
	if (!isAnyBotAlive)
	{
		PrepareForNextWave();
	}
}
