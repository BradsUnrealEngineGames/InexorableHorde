// Bradley Olson - GNU General Public License v3.0

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "IHPickupActor.generated.h"

class USphereComponent;
class UDecalComponent;
class AIHPowerupActor;

UCLASS()
class INEXORABLEHORDE_API AIHPickupActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AIHPickupActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	USphereComponent* SphereComp;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UDecalComponent* DecalComp;

	UPROPERTY(EditInstanceOnly, Category = "Properties")
	TSubclassOf<AIHPowerupActor> PowerupClass;

	AIHPowerupActor* PowerupInstance;

	void Respawn();

	UPROPERTY(EditAnywhere, Category = "Properties")
	float CooldownDuration;

	FTimerHandle TimerHandle_PowerupRespawn;

public:
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
};
