// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "SActionEffect.h"
#include "SProjectileBase.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "SMagicProjectile.generated.h"

UCLASS()
class ACTIONROUGELIKE_API ASMagicProjectile : public ASProjectileBase
{
	GENERATED_BODY()

	// protected:
	// 	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	// 	USphereComponent* SphereComp; // 球体组件，作为碰撞组件
	//
	// 	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	// 	UProjectileMovementComponent* MovementComp; // 抛射物运动组件
	//
	// 	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	// 	UParticleSystemComponent* EffectComp; // 粒子系统组件

public:
	// Sets default values for this actor's properties
	ASMagicProjectile();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnActorOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	                    int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	virtual void OnActorHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse,
	                        const FHitResult& Hit) override;
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float DamageAmount;

	UPROPERTY(EditDefaultsOnly, Category = "Effects")
	TSubclassOf<UCameraShakeBase> CameraShake;

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	FGameplayTag ParryTag; // 反弹buff

	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	TSubclassOf<USActionEffect> BurningActionClass;
};
