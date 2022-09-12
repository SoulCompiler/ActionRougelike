// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SAIController.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"

void ASAIController::BeginPlay()
{
	Super::BeginPlay();

	if (ensureMsgf(BehaviorTree, TEXT("BehaviorTree is nullptr! Please assign BehaviorTree in your AI Controller.")))
	{
		RunBehaviorTree(BehaviorTree);
	}


	// APawn* PlayerPawn =  UGameplayStatics::GetPlayerPawn(this,0);
	// if (ensure(PlayerPawn))
	// {
	// 	// 此处用的是硬编码。
	// 	// 在C++中定义变量，再在蓝图中使用，就可以消除硬编码。
	// 	GetBlackboardComponent()->SetValueAsVector("MoveToLocation",PlayerPawn->GetActorLocation());	
	// 	
	// 	GetBlackboardComponent()->SetValueAsObject("TargetActor",PlayerPawn);
	// 	
	// }
}
