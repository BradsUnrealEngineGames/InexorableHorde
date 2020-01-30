// Bradley Olson - GNU General Public License v3.0


#include "IHPowerupActor.h"
#include "TimerManager.h"
#include "Net/UnrealNetwork.h"
#include "SCharacter.h"

// Sets default values
AIHPowerupActor::AIHPowerupActor()
{
	PowerupInterval = 0.0f;
	TotalNumberOfTicks = 0;
	TicksProcessed = 0;

	SetReplicates(true);
}

void AIHPowerupActor::OnTickPowerup()
{
	if (Role != ROLE_Authority) { return; }

	TicksProcessed++;

	OnPowerupTicked();

	if (TotalNumberOfTicks <= TicksProcessed)
	{
		OnExpired();

		GetWorldTimerManager().ClearTimer(TimerHandle_PowerupTick);
	}
}

void AIHPowerupActor::OnRep_PowerupActive()
{
	OnPowerupStateChanged(IsPowerupActive);
}

void AIHPowerupActor::ActivatePowerup(ASCharacter* ActivatingCharacter)
{
	IsPowerupActive = true;
	OnRep_PowerupActive();

	OnActivated(ActivatingCharacter);

	if (PowerupInterval > 0.0f)
	{
		GetWorldTimerManager().SetTimer(TimerHandle_PowerupTick, this, &AIHPowerupActor::OnTickPowerup, PowerupInterval, true, 0.0f);
	}
	else
	{
		OnTickPowerup();
	}
}

void AIHPowerupActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AIHPowerupActor, IsPowerupActive);
}
