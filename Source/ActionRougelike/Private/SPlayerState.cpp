// Fill out your copyright notice in the Description page of Project Settings.


#include "SPlayerState.h"

#include "Net/UnrealNetwork.h"

ASPlayerState::ASPlayerState()
{
	Credits = 0.0f;
}

float ASPlayerState::GetCredits() const
{
	return Credits;
}

bool ASPlayerState::ApplyCreditsChange(AActor* InstigatorActor, float Delta)
{
	const float NewCredits = Credits + Delta;

	if (NewCredits < 0.0f)
	{
		return false;
	}

	if (GetOwner()->HasAuthority())
	{
		Credits = NewCredits;

		MulticastCreditsChanged(NewCredits, Delta);
	}

	return true;
}

void ASPlayerState::MulticastCreditsChanged_Implementation(float NewCredits, float Delta)
{
	OnCreditsChanged.Broadcast(this, NewCredits, Delta);
}

void ASPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASPlayerState, Credits);
}
