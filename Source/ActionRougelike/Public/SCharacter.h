// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SActionComponent.h"
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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Conponents")
	USActionComponent* ActionComp;

	virtual void PostInitializeComponents() override;

	void MoveForward(float Value);

	void MoveRight(float Value);

	void SprintStart();

	void SprintStop();

	void PrimaryAttack();

	void PrimaryInteract();

	void BlackHoleAttack();

	void Dash();

	UFUNCTION()
	void OnHealthChanged(AActor* InstigatorActor, USAttributeComponent* OwningComp, float NewHealth, float Delta);

	// 提供给InteractionComponent的GetActorEyesViewPoint()用于射线追踪，GetActorEyesViewPoint()的内部实现实际上是用了
	// override的GetPawnViewLocation()来获取StartLocation，这里再override一下即可
	virtual FVector GetPawnViewLocation() const override;
	
};
