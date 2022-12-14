// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SPowerupActorBase.h"
#include "SPowerup_HealthPotion.generated.h"

UCLASS()
class ACTIONROUGELIKE_API ASPowerup_HealthPotion : public ASPowerupActorBase
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ASPowerup_HealthPotion();

	virtual void Interact_Implementation(APawn* InstigatorPawn) override;

	virtual FText GetInteractText_Implementation(APawn* InstigatorPawn) override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Healing")
	float HealingAmount;
};
