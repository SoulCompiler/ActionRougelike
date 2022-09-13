// Fill out your copyright notice in the Description page of Project Settings.


#include "SAttributeComponent.h"

// Sets default values for this component's properties
USAttributeComponent::USAttributeComponent()
{
	HealthMax = 100;
	Health = 100;
}

bool USAttributeComponent::ApplyHealthChange(float Delta)
{
	// Health += Delta;
	// Health =  FMath::Clamp(Health, 0.0f, HealthMax);

	float OldHealth = Health;

	Health = FMath::Clamp(Health + Delta, 0.0f, HealthMax);

	float ActualDelta = Health - OldHealth;
	OnHealthChanged.Broadcast(nullptr, this, Health, ActualDelta);

	return ActualDelta != 0; // 返回bool类型变量是为了之后判断这个生命值改变是否有效而准备的。
}

bool USAttributeComponent::IsAlive() const
{
	return Health > 0.0f;
}

bool USAttributeComponent::IsFullHealth() const
{
	return FMath::IsNearlyEqual(Health, HealthMax);
}

bool USAttributeComponent::IsLowHealth() const
{
	return Health <= 30.0f;
}
