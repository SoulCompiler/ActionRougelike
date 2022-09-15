// Fill out your copyright notice in the Description page of Project Settings.


#include "SPowerup_Credits.h"

#include "SPlayerState.h"

ASPowerup_Credits::ASPowerup_Credits()
{

	CoolDownTime = 10.0f;
}

void ASPowerup_Credits::Interact_Implementation(APawn* InstigatorPawn)
{
	ASPlayerState* InstigatorState = InstigatorPawn->GetPlayerState<ASPlayerState>();
	if (ensure(InstigatorState))
	{
		// @todo: 改进硬编码
		InstigatorState->ApplyCreditsChange(30);
		
		HideAndCoolDownPowerup();
	}
}

