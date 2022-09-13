// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "SBTTask_RestoreHealth.generated.h"

/**
 * 
 */
UCLASS()
class ACTIONROUGELIKE_API USBTTask_RestoreHealth : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	UPROPERTY(EditAnywhere,Category = "Attributes")
	float HealingAmount;

	UPROPERTY(EditAnywhere,Category = "AI")
	FBlackboardKeySelector LowHealthKey;
};
