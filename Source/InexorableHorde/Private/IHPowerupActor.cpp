// Bradley Olson - GNU General Public License v3.0


#include "IHPowerupActor.h"
#include "TimerManager.h"

// Sets default values
AIHPowerupActor::AIHPowerupActor()
{
	PowerupInterval = 0.0f;
	TotalNumberOfTicks = 0;
	TicksProcessed = 0;
}

// Called when the game starts or when spawned
void AIHPowerupActor::BeginPlay()
{
	Super::BeginPlay();
	
	
}

void AIHPowerupActor::OnTickPowerup()
{
	TicksProcessed++;

	OnPowerupTicked();

	if (TotalNumberOfTicks >= TicksProcessed)
	{
		OnExpired();

		GetWorldTimerManager().ClearTimer(TimerHandle_PowerupTick);
	}
}

void AIHPowerupActor::ActivatePowerup()
{

	OnActivated();

	if (PowerupInterval > 0.0f)
	{
		GetWorldTimerManager().SetTimer(TimerHandle_PowerupTick, this, &AIHPowerupActor::OnTickPowerup, PowerupInterval, true, 0.0f);
	}
	else
	{
		OnTickPowerup();
	}
}
