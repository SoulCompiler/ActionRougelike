// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "SAction.generated.h"

// 用于网络同步的变量包
USTRUCT()
struct FActionRepData
{
	GENERATED_BODY()

public:
	UPROPERTY()
	bool bIsRunning;

	// UPROPERTY(NotReplicated)		// 表示该变量不需要同步
	UPROPERTY()
	AActor* Instigator;
};


class USActionComponent;

/**
 * 
 */
UCLASS(Blueprintable) // 可作为蓝图的基类
class ACTIONROUGELIKE_API USAction : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, Category = "Action")
	void StartAction(AActor* Instigator);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Action")
	void StopAction(AActor* Instigator);

	// 为什么要override GetWorld呢？
	virtual UWorld* GetWorld() const override;

	UFUNCTION(BlueprintNativeEvent, Category = "Action")
	bool CanStart(AActor* Instigator);

	UFUNCTION(BlueprintCallable, Category = "Action")
	bool IsRunning() const;

	virtual bool IsSupportedForNetworking() const override
	{
		return true;
	}

	void Initialize(USActionComponent* NewActionComp);

protected:
	UFUNCTION(BlueprintCallable, Category = "Action")
	USActionComponent* GetOwningComponent() const;

	UFUNCTION()
	void OnRep_RepData();


public:
	/* Action nickname to start/stop without a reference to the object */
	UPROPERTY(EditDefaultsOnly, Category = "Action")
	FName ActionName;

	// Start immediately when added to an action component
	UPROPERTY(EditDefaultsOnly, Category = "Action")
	bool bAutoStart;

protected:
	/* Tags added to owning actor when activated,removed when action stops */
	UPROPERTY(EditAnywhere, Category = "Tags")
	FGameplayTagContainer GrantsTags;
	/* Action can only start if owningActor has none of these Tags applied */
	UPROPERTY(EditAnywhere, Category = "Tags")
	FGameplayTagContainer BlockedTags;

	UPROPERTY(ReplicatedUsing="OnRep_RepData")
	// bool bIsRunning;
	/*	↓
		↓
		↓	*/
	FActionRepData RepData;

	UPROPERTY(Replicated)
	USActionComponent* ActionComp;
};
