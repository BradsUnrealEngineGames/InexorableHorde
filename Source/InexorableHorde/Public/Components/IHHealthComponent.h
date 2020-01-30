// Bradley Olson - GNU General Public License v3.0

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "IHHealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_SixParams(FOnHealthChangedSignature, UIHHealthComponent*, HealthComp, float, Health, float, HealthDelta, const class UDamageType*, DamageType, class AController*, InstigatedBy, AActor*, DamageCauser);

UCLASS( ClassGroup=(COOP), meta=(BlueprintSpawnableComponent) )
class INEXORABLEHORDE_API UIHHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UIHHealthComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(ReplicatedUsing=OnRep_Health, BlueprintReadWrite, Category = "HealthComponent")
	float Health;

	UFUNCTION()
	void OnRep_Health(float OldHealth);

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "HealthComponent")
	float MaxHealth;

	UFUNCTION()
	void HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

public:	

	float GetHealth() const;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnHealthChangedSignature OnHealthChanged;

	UFUNCTION(BlueprintCallable, Category = "HealthComponent")
	void Heal(float HealAmount);
		
};
