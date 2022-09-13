// Fill out your copyright notice in the Description page of Project Settings.


#include "SPotionBase.h"

// Sets default values
ASPotionBase::ASPotionBase()
{
	SphereComp = CreateDefaultSubobject<USphereComponent>("SphereComp");
	SphereComp->SetCollisionProfileName("Potion");
	RootComponent = SphereComp;

	CoolDownTime = 10.f;
}

void ASPotionBase::Interact_Implementation(APawn* InstigatorPawn)
{
}

void ASPotionBase::ReSpawnPotion()
{
	SetPotionState(true);
}

void ASPotionBase::HideAndCoolDownPotion()
{
	SetPotionState(false);

	GetWorldTimerManager().SetTimer(TimerHandle_ReSpawn,this,&ASPotionBase::ReSpawnPotion,CoolDownTime);
}

void ASPotionBase::SetPotionState(bool bIsActive)
{
	SetActorEnableCollision(bIsActive);

	// Set Visibility on root and all children
	RootComponent->SetVisibility(bIsActive,true);
}
