// Bradley Olson - GNU General Public License v3.0

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ExplosiveBarrel.generated.h"

class UStaticMeshComponent;
class UIHHealthComponent;
class URadialForceComponent;
class UMaterial;
class UParticleSystem;
class UDamageType;

UCLASS()
class INEXORABLEHORDE_API AExplosiveBarrel : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AExplosiveBarrel();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Components")
	UStaticMeshComponent* MeshComp =  nullptr;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Components")
	UIHHealthComponent* HealthComp = nullptr;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Components")
	URadialForceComponent* RadialForceComp = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Extras")
	UMaterial* MatExploded = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Extras")
	UParticleSystem* ExplosionSystem;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<UDamageType> ExplosionDamageType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Extras")
	float ExplosionDamage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Extras")
	float ExplosionRadius;



	UFUNCTION()
	void OnHealthChanged(UIHHealthComponent* OwnerHealthComp, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	bool bUnexploded;

public:	

};
