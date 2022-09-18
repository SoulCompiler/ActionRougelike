// Fill out your copyright notice in the Description page of Project Settings.


#include "SAttributeComponent.h"

#include "SGameModeBase.h"
#include "Net/UnrealNetwork.h"

// 控制台变量，伤害系数
static TAutoConsoleVariable<float> CVarDamageMultiplier(TEXT("su.DamageMultiplier"), 1.0f,TEXT("Global Damage Modifier for Attribute Component."), ECVF_Cheat);

// Sets default values for this component's properties
USAttributeComponent::USAttributeComponent()
{
	HealthMax = 100.0f;
	Health = 100.0f;
	RageMax = 100.0f;
	Rage = 0.0f;

	SetIsReplicatedByDefault(true);
}

USAttributeComponent* USAttributeComponent::GetAttributes(AActor* FromActor)
{
	if (FromActor)
	{
		return Cast<USAttributeComponent>(FromActor->GetComponentByClass(USAttributeComponent::StaticClass()));
	}

	return nullptr;
}

bool USAttributeComponent::IsActorAlive(AActor* Actor)
{
	USAttributeComponent* AttributeComp = GetAttributes(Actor);
	if (AttributeComp)
	{
		return AttributeComp->IsAlive();
	}

	return false;
}

bool USAttributeComponent::ApplyHealthChange(AActor* InstigatorActor, float Delta)
{
	// 用UE自带的作弊器相关函数CanBeDamaged()来具体实现作弊功能
	if (!GetOwner()->CanBeDamaged() && Delta < 0.0f /* 这个Delta有意义吗？都上帝模式了还在乎是否恢复血量？*/)
	{
		return false;
	}

	if (Delta < 0.0f)
	{
		float DamageMultiplier = CVarDamageMultiplier.GetValueOnGameThread();
		Delta *= DamageMultiplier;

		ApplyRageChange(InstigatorActor, FMath::Abs(Delta));
	}

	float OldHealth = Health;

	Health = FMath::Clamp(Health + Delta, 0.0f, HealthMax);

	float ActualDelta = Health - OldHealth;

	// @Todo: 改进以满足某些调用需要完整的伤害数值，比如伤害文本需要Delta而不是ActualDelta
	// OnHealthChanged.Broadcast(InstigatorActor, this, Health, ActualDelta);

	if (ActualDelta != 0.0f)
	{
		MulticastHealthChanged(InstigatorActor, Health, ActualDelta);
	}

	// Died
	if (ActualDelta < 0.0f && Health == 0.0f)
	{
		ASGameModeBase* GM = GetWorld()->GetAuthGameMode<ASGameModeBase>();
		if (GM)
		{
			// @fixme：如果是第三方物品让角色死亡呢？
			GM->OnActorKilled(GetOwner(), InstigatorActor);
		}
	}

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
	// @Todo:用百分比生命值替代
	return Health <= 30.0f;
}

bool USAttributeComponent::Kill(AActor* InstigatorActor)
{
	return ApplyHealthChange(InstigatorActor, -GetHealthMax());
}

float USAttributeComponent::GetHealthMax() const
{
	return HealthMax;
}

bool USAttributeComponent::ApplyRageChange(AActor* InstigatorActor, float Delta)
{
	const float OldRage = Rage;
	const float NewRage = OldRage + Delta;

	if (NewRage < 0.0f)
	{
		return false;
	}

	Rage = FMath::Clamp(NewRage, 0.0f, RageMax);

	const float ActualDelta = Rage - OldRage;
	OnRageChanged.Broadcast(InstigatorActor, this, Rage, ActualDelta);

	return true;
}

void USAttributeComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(USAttributeComponent, Health);
	DOREPLIFETIME(USAttributeComponent, HealthMax);

	// 带条件的变量同步，当该变量更新时，只有主人才会看到变化。可以优化性能
	// DOREPLIFETIME_CONDITION(USAttributeComponent, HealthMax, COND_OwnerOnly);
}


void USAttributeComponent::MulticastHealthChanged_Implementation(AActor* InstigatorActor, float NewHealth, float Delta)
{
	OnHealthChanged.Broadcast(InstigatorActor, this, NewHealth, Delta);
}
