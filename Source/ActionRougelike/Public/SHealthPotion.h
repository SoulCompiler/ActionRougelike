// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SPotionBase.h"
#include "SHealthPotion.generated.h"

UCLASS()
class ACTIONROUGELIKE_API ASHealthPotion : public ASPotionBase
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ASHealthPotion();

protected:
	virtual void Interact_Implementation(APawn* InstigatorPawn) override;

protected:
	UPROPERTY(VisibleAnywhere, Category = "Component")
	UStaticMeshComponent* MeshComp;

	UPROPERTY(EditAnywhere, Category = "Healing")
	float HealingAmount;
};
