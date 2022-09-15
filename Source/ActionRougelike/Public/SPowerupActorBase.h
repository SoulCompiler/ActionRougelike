// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SGameplayInterface.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include "SPowerupActorBase.generated.h"

UCLASS()
class ACTIONROUGELIKE_API ASPowerupActorBase : public AActor, public ISGameplayInterface
{
	GENERATED_BODY()

public:
	ASPowerupActorBase();

protected:
	virtual void Interact_Implementation(APawn* InstigatorPawn) override;

	void ReSpawnPowerup();

	void HideAndCoolDownPowerup();

	void SetPowerupState(bool bIsActive);

protected:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	USphereComponent* SphereComp;

	UPROPERTY(VisibleAnywhere, Category = "Component")
	UStaticMeshComponent* MeshComp;

	UPROPERTY(EditAnywhere, Category = "Powerup")
	float CoolDownTime;

	FTimerHandle TimerHandle_ReSpawn;
};
