// Fill out your copyright notice in the Description page of Project Settings.


#include "SPowerup_Credits.h"

#include "SPlayerState.h"

ASPowerup_Credits::ASPowerup_Credits()
{
	CoolDownTime = 10.0f;

	CreditsCost = -30.0f;
}

void ASPowerup_Credits::Interact_Implementation(APawn* InstigatorPawn)
{
	ASPlayerState* InstigatorState = InstigatorPawn->GetPlayerState<ASPlayerState>();
	if (ensure(InstigatorState))
	{
		InstigatorState->ApplyCreditsChange(this, -CreditsCost);

		HideAndCoolDownPowerup();
	}
}
