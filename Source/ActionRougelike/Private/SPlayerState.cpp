// Fill out your copyright notice in the Description page of Project Settings.


#include "SPlayerState.h"

ASPlayerState::ASPlayerState()
{
	Credits = 0.0f;
}

float ASPlayerState::GetCredits() const
{
	return Credits;
}

bool ASPlayerState::ApplyCreditsChange(float Delta)
{
	const float NewCredits = Credits + Delta;

	if (NewCredits < 0.0f)
	{
		return false;
	}

	Credits = NewCredits;

	OnCreditsChanged.Broadcast(this, NewCredits, Delta);
	return true;
}
