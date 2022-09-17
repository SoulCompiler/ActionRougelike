// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "SAction.generated.h"

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


protected:
	UFUNCTION(BlueprintCallable, Category = "Action")
	USActionComponent* GetOwningComponent() const;

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

	bool bIsRunning;
};
