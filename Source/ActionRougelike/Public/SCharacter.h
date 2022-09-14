// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SAttributeComponent.h"
#include "SInteractionComponent.h"
#include "SProjectileBase.h"
#include "GameFramework/Character.h"
#include "SCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class USInteractionComponent;
class UAnimMontage;
class USAttributeComponent;

UCLASS()
class ACTIONROUGELIKE_API ASCharacter : public ACharacter
{
	GENERATED_BODY()


public:
	// Sets default values for this character's properties
	ASCharacter();
	
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// 在控制台中输入执行
	UFUNCTION(Exec)
	void HealSelf(float Amount = 100.0f);

protected:
	UPROPERTY(VisibleAnywhere) // 属性系统中UPROPERTY()，为变量添加在编辑器中可见的属性
	USpringArmComponent* SpringArmComp;
	// 为什么弹簧臂组件要和相机组件绑定使用？答案：用SpringArm控制摄像机的Pitch，Yaw，Roll。在此例中，SpringArm还代表ControllerRotation

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* CameraComp;

	UPROPERTY(VisibleAnywhere)
	USInteractionComponent* InteractionComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USAttributeComponent* AttributeComp;

	virtual void PostInitializeComponents() override;
	

	void MoveForward(float Value);

	void MoveRight(float Value);

	void PrimaryAttack();

	void PrimaryAttack_TimeElapsed();

	void PrimaryInteract();

	void BlackHoleAttack();

	void BlackHoleAttack_TimeElapsed();

	void Dash();

	void Dash_TimeElaped();

	void SpawnProjectile(TSubclassOf<AActor> ClassToSpawn);

	UFUNCTION()
	void OnHealthChanged(AActor* InstigatorActor, USAttributeComponent* OwningComp, float NewHealth, float Delta);

protected:
	UPROPERTY(EditAnywhere, Category = "Attack") // Category为变量在编辑器中的分布进行分类
	TSubclassOf<AActor> ProjectileClass;

	UPROPERTY(EditAnywhere, Category = "Attack")
	TSubclassOf<AActor> BlackHoleProjectileClass;

	UPROPERTY(EditAnywhere, Category = "Attack")
	TSubclassOf<AActor> DashProjectileClass;

	UPROPERTY(EditAnywhere, Category = "Attack")
	UAnimMontage* AttackAnim;

	UPROPERTY(EditAnywhere, Category = "Attack")
	UParticleSystem* AttackVFX;

	FTimerHandle TimerHandle_PrimaryAttack;

	FTimerHandle TimerHandle_BlackHoleAttack;

	FTimerHandle TimerHandle_Dash;

	UPROPERTY(EditAnywhere, Category = "Attack")
	float FiringRange;

	float AttackAnimDelay;

	UPROPERTY(VisibleAnywhere,Category = "Effects")
	FName HandSocketName;
};
