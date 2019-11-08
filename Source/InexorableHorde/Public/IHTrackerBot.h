// Bradley Olson - GNU General Public License v3.0

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "IHTrackerBot.generated.h"

class UIHHealthComponent;

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

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Components")
	UIHHealthComponent* HealthComp = nullptr;

	UFUNCTION()
	void HandleTakeDamage(UIHHealthComponent* OwningHealthComp, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

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

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
