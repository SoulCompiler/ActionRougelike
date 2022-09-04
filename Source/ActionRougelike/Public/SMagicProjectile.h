// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "SMagicProjectile.generated.h"

UCLASS()
class ACTIONROUGELIKE_API ASMagicProjectile : public AActor
{
	GENERATED_BODY()
	
public:	

	UPROPERTY(VisibleAnywhere)
	USphereComponent* SphereComp;		// 球体组件，作为碰撞组件

	UPROPERTY(VisibleAnywhere)
	UProjectileMovementComponent* MovementComp;		// 抛射物运动组件

	UPROPERTY(VisibleAnywhere)
	UParticleSystemComponent* EffectComp;		// 粒子系统组件

	// Sets default values for this actor's properties
	ASMagicProjectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
