// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SGameplayInterface.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include "SPotionBase.generated.h"

UCLASS()
class ACTIONROUGELIKE_API ASPotionBase : public AActor, public ISGameplayInterface
{
	GENERATED_BODY()

public:
	ASPotionBase();

protected:
	virtual void Interact_Implementation(APawn* InstigatorPawn) override;

	void ReSpawnPotion();

	void HideAndCoolDownPotion();

	void SetPotionState(bool bIsActive);

protected:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	USphereComponent* SphereComp;

	UPROPERTY(EditAnywhere, Category = "Potion")
	float CoolDownTime;

	FTimerHandle TimerHandle_ReSpawn;
};
