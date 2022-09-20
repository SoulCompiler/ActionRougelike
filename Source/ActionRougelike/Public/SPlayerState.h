// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SSaveGame.h"
#include "GameFramework/PlayerState.h"
#include "SPlayerState.generated.h"

class ASPlayerState;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnCreditsChanged, ASPlayerState*, PlayerState, float, NewCredits, float, Delta);

/**
 * 
 */
UCLASS()
class ACTIONROUGELIKE_API ASPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	ASPlayerState();

	UFUNCTION(BlueprintCallable, Category = "Credits")
	float GetCredits() const;

	UFUNCTION(BlueprintCallable, Category = "Credits")
	bool ApplyCreditsChange(AActor* InstigatorActor, float Delta);

	UFUNCTION(BlueprintNativeEvent)
	void SavePlayerState(USSaveGame* SaveObject);

	UFUNCTION(BlueprintNativeEvent)
	void LoadPlayerState(USSaveGame* SaveObject);
protected:
	// 在这里使用multicast的坏处是我们会通过网络发送过多的数据, 因为RPC至少要发送两个参数. OnRep_ 回调函数则没有额外开销因为反正我们要复制得分
	// UFUNCTION(NetMulticast, Reliable) // @FIXME: mark as unreliable once we move the 'state' our of scharacter
	// void MulticastCreditsChanged(float NewCredits, float Delta);

	// OnRep_ 可以添加一个参数用来引用他所绑定的变量的旧值，在这里对于我们计算出分数差值很有用
	UFUNCTION()
	void OnRep_Credits(float OldCredits /* 自动绑定旧的Credits，类型必须相同 */);

protected:
	UPROPERTY(ReplicatedUsing="OnRep_Credits", VisibleAnywhere, Category = "Credits")
	float Credits;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnCreditsChanged OnCreditsChanged;
};
