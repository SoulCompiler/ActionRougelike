// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "SProjectileBase.generated.h"

UCLASS()
class ACTIONROUGELIKE_API ASProjectileBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASProjectileBase();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly,Category = "Components")
	USphereComponent* SphereComp; // 球体组件，作为碰撞组件

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly,Category = "Components")
	UProjectileMovementComponent* MoveComp; // 抛射物运动组件

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly,Category = "Components")
	UParticleSystemComponent* EffectComp; // 粒子系统组件

	UPROPERTY(EditDefaultsOnly,Category = "Effects")
	UParticleSystem* ImpactVFX;	// 粒子特效

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category = "Components")
	UAudioComponent* AudioComp;	// 音频组件

	UPROPERTY(EditDefaultsOnly,Category = "Effects")
	USoundBase* ImpactAFX;

protected:
	UFUNCTION()
	virtual void OnActorHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
								   UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);
	
	UFUNCTION(BlueprintCallable,BlueprintNativeEvent)
	void Explode();

	
	virtual void PostInitializeComponents() override;
};
