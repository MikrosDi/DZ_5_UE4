// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DamageTarget.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TANKPROJECT_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

	DECLARE_EVENT(UHealthComponent,FDieEvent)
	DECLARE_EVENT_OneParam(UHealthComponent,FHealthChanged, float)

public:
	// Sets default values for this component's properties
	UHealthComponent();

	void TakeDamage(FDamageData DamageData);
	
	float GetHeath() const
	{
		return CurrentHealth;
	}
	FDieEvent OnDeath;
	FHealthChanged OnHealthChanged;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite)
	float MaxHealth = 100;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite)
	float CurrentHealth;
};

