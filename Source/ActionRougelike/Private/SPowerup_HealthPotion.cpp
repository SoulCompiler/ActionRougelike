// Fill out your copyright notice in the Description page of Project Settings.


#include "SPowerup_HealthPotion.h"

#include "SCharacter.h"
#include "SPlayerState.h"
#include "GameFramework/Character.h"

#define LOCTEXT_NAMESPACE "InteractableActors"

ASPowerup_HealthPotion::ASPowerup_HealthPotion()
{
	HealingAmount = 20.0f;
	CoolDownTime = 10.0f;
	CreditsCost = 20.0f;
}

void ASPowerup_HealthPotion::Interact_Implementation(APawn* InstigatorPawn)
{
	if (!ensure(InstigatorPawn))
	{
		return;
	}

	// USAttributeComponent* AttributeComp = Cast<USAttributeComponent>(InstigatorPawn->GetComponentByClass(USAttributeComponent::StaticClass()));
	// 由静态函数替代
	USAttributeComponent* AttributeComp = USAttributeComponent::GetAttributes(InstigatorPawn);
	ASPlayerState* InstigatorState = InstigatorPawn->GetPlayerState<ASPlayerState>();
	if (ensure(InstigatorState) && ensure(AttributeComp) && !AttributeComp->IsFullHealth())
	{
		if (InstigatorState->ApplyCreditsChange(this, -CreditsCost))
		{
			// if (AttributeComp->ApplyHealthChange(this, HealingAmount))
			// 不管血是不是满的都加
			AttributeComp->ApplyHealthChange(this, HealingAmount);

			HideAndCoolDownPowerup();
		}
	}
}

FText ASPowerup_HealthPotion::GetInteractText_Implementation(APawn* InstigatorPawn)
{
	USAttributeComponent* AttributeComp = USAttributeComponent::GetAttributes(InstigatorPawn);
	if (AttributeComp && AttributeComp->IsFullHealth())
	{
		return LOCTEXT("HealthPotion_FullHealthWarning", "Already at full health");
	}

	return FText::Format(LOCTEXT("HealthPotion_InteractMessage", "Cost {0} Credits. Restores health to maximum."), CreditsCost);
}

#undef LOCTEXT_NAMESPACE