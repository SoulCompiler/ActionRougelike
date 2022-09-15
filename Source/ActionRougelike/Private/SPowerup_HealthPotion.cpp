// Fill out your copyright notice in the Description page of Project Settings.


#include "SPowerup_HealthPotion.h"

#include "SCharacter.h"
#include "GameFramework/Character.h"

// Sets default values
ASPowerup_HealthPotion::ASPowerup_HealthPotion()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	// PrimaryActorTick.bCanEverTick = true;

	HealingAmount = 20.0f;
	CoolDownTime = 10.0f;
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
		// @todo: 改进硬编码
		if (InstigatorState->ApplyCreditsChange(-20.0f) && AttributeComp->ApplyHealthChange(this, HealingAmount))
		{
			HideAndCoolDownPowerup();
		}
	}
}
