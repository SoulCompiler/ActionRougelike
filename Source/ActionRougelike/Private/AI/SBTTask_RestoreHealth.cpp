// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SBTTask_RestoreHealth.h"

#include "AIController.h"
#include "SAttributeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"

EBTNodeResult::Type USBTTask_RestoreHealth::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* MyController = OwnerComp.GetAIOwner();
	if (ensure(MyController))
	{
		ACharacter* MyPawn = Cast<ACharacter>(MyController->GetPawn());
		if (MyPawn == nullptr)
		{
			return EBTNodeResult::Failed;
		}

		// USAttributeComponent* AttributeComp = Cast<USAttributeComponent>(MyPawn->GetComponentByClass(USAttributeComponent::StaticClass()));
		// 由静态函数替代
		USAttributeComponent* AttributeComp = USAttributeComponent::GetAttributes(MyPawn);

		if (ensure(AttributeComp))
		{
			if (AttributeComp->IsAlive())
			{
				AttributeComp->ApplyHealthChange(MyPawn, HealingAmount);

				UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
				if (ensure(Blackboard))
				{
					Blackboard->SetValueAsBool(LowHealthKey.SelectedKeyName, false);

					return EBTNodeResult::Succeeded;
				}
			}
		}
	}

	return EBTNodeResult::Failed;
}
