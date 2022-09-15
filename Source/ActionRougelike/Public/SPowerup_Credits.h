// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SPowerupActorBase.h"
#include "SPowerup_Credits.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONROUGELIKE_API ASPowerup_Credits : public ASPowerupActorBase
{
	GENERATED_BODY()

public:
	ASPowerup_Credits();

protected:
	virtual void Interact_Implementation(APawn* InstigatorPawn) override;
};
