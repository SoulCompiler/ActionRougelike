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

		USAttributeComponent* AttributeComp = Cast<USAttributeComponent>(MyPawn->GetComponentByClass(USAttributeComponent::StaticClass()));
		if (ensure(AttributeComp))
		{
			if (AttributeComp->IsAlive())
			{
				AttributeComp->ApplyHealthChange(HealingAmount);

				UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
				if (ensure(Blackboard))
				{
					Blackboard->SetValueAsBool(LowHealthKey.SelectedKeyName,false);

					return EBTNodeResult::Succeeded;
				}
			}
		}
	}

	return EBTNodeResult::Failed;
}
