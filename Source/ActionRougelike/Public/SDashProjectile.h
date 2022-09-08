// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "SDashProjectile.generated.h"

UCLASS()
class ACTIONROUGELIKE_API ASDashProjectile : public AActor
{
	GENERATED_BODY()
	
public:
	
	// Sets default values for this actor's properties
	ASDashProjectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	void Projectile_Explode();
	
	void Dash_Effect();
    	
public:
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	USphereComponent* SphereComp;		// 球体组件，作为碰撞组件

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	UProjectileMovementComponent* MovementComp;		// 抛射物运动组件

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	UParticleSystemComponent* EffectComp;		// 粒子系统组件
	
	FTimerHandle TimerHandle_Dash;

	UPROPERTY(EditAnywhere,Category = "Emitter")
	UParticleSystem* EmitterTemplate;
};
