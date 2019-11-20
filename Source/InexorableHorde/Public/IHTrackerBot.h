// Bradley Olson - GNU General Public License v3.0

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "IHTrackerBot.generated.h"

class UIHHealthComponent;
class USphereComponent;
class USoundCue;

UCLASS()
class INEXORABLEHORDE_API AIHTrackerBot : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AIHTrackerBot();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	UStaticMeshComponent* MeshComp = nullptr;

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	UIHHealthComponent* HealthComp = nullptr;

	UPROPERTY(VisibleDefaultsOnly, Category = "Components")
	USphereComponent* OverlapSphereComp = nullptr;

	UFUNCTION()
	void HandleTakeDamage(UIHHealthComponent* OwningHealthComp, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	UFUNCTION()
	void SmallSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	FVector GetNextPathPoint();

	// Next point in navigation path
	FVector NextPathPoint;

	UPROPERTY(EditDefaultsOnly, Category = "Properties", meta = (ClampMin = 1, ClampMax = 1000))
	float MovementForce;

	UPROPERTY(EditDefaultsOnly, Category = "Properties")
	bool bUseVelocityChange;

	UPROPERTY(EditDefaultsOnly, Category = "Properties", meta = (ClampMin = 1, ClampMax = 1000))
	float RequiredDistanceToTarget;

	// Dynamic material to pulse on damage
	UMaterialInstanceDynamic* MatInst = nullptr;

	void SelfDestruct();

	UPROPERTY(EditDefaultsOnly, Category = "Properties")
	UParticleSystem* ExplosionEffect = nullptr;

	bool bExploded;

	bool bStartedSelfDestruction;

	UPROPERTY(EditDefaultsOnly, Category = "Properties", meta = (ClampMin = 1, ClampMax = 1000))
	float ExplosionRadius;

	UPROPERTY(EditDefaultsOnly, Category = "Properties")
	float ExplosionDamage;

	FTimerHandle TimerHandle_SelfDamage;

	void DamageSelf();

	FTimerHandle TimerHandle_SwarmCheck;

	UPROPERTY(EditDefaultsOnly, Category = "Properties")
	USoundCue* SelfDestructSound = nullptr;
	
	UPROPERTY(EditDefaultsOnly, Category = "Properties")
	USoundCue* ExplosionSound = nullptr;

	UFUNCTION()
	void OnCheckNearbyBots();

	int32 PowerLevel;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
