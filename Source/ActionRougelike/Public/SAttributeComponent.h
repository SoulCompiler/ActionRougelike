// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SAttributeComponent.generated.h"


class USAttributeComponent;
// 动态组播
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnHealthChanged, AActor*, InstigatorActor, USAttributeComponent*,
                                              OwningComp, float, NewHealth, float, Delta);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnRageChanged, AActor*, InstigatorActor, USAttributeComponent*,
                                              OwningComp, float, NewRage, float, Delta);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ACTIONROUGELIKE_API USAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	USAttributeComponent();

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	static USAttributeComponent* GetAttributes(AActor* FromActor);

	UFUNCTION(BlueprintCallable, Category = "Attributes", meta = (DisplayName = "IsAlive")) // 类的成员变量（属性）标记 DisplayName 的 meta 后，搜索时可能更为便捷，可以不使用原本的成员变量关键字。
	static bool IsActorAlive(AActor* Actor);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Replicated, Category = "Attributes")
	// EditDefaultOnly表示该变量只能在蓝图的属性窗口编辑，不能在level的属性窗口编辑。
	float Health;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Replicated, Category = "Attributes")
	float HealthMax;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes")
	float Rage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes")
	float RageMax;

	// UPROPERTY(ReplicatedUsing="")
	// bool bIsAlive;

public:
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnHealthChanged OnHealthChanged; // 动态组播的代理名称

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnRageChanged OnRageChanged;

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	bool ApplyHealthChange(AActor* InstigatorActor, float Delta);

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	bool IsAlive() const;

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	bool IsFullHealth() const;

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	bool IsLowHealth() const;

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	bool Kill(AActor* InstigatorActor);

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	float GetHealthMax() const;

	UFUNCTION(BlueprintCallable, Category = "Attributes")
	bool ApplyRageChange(AActor* InstigatorActor, float Delta);

protected:
	// 因为Health变量已经被同步了，所以这个装饰变量被同步后的反应的函数可以是Unreliable的；
	// 但是，因为这个函数的实现时组播OnHealthChanged这个事件的发生，假如某个绑定该事件的函数是非常重要的会影响游戏状态的，
	// 比如SCharacter.cpp中的OnHealthChanged就处理了玩家死亡的逻辑，这是非常重要的。这时这个函数就必须是Reliable的
	UFUNCTION(NetMulticast, Reliable)	// @FIXME: mark as unreliable once we move the 'state' our of scharacter
	void MulticastHealthChanged(AActor* InstigatorActor, float NewHealth, float Delta);
};
