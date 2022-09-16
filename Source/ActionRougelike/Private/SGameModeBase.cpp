// Fill out your copyright notice in the Description page of Project Settings.


#include "SGameModeBase.h"

#include "AITypes.h"
#include "DrawDebugHelpers.h"
#include "EngineUtils.h"
#include "SAttributeComponent.h"
#include "SCharacter.h"
#include "SPlayerState.h"
#include "AI/SAICharacter.h"
#include "EnvironmentQuery/EnvQueryManager.h"

// 控制台变量，控制AI生成
static TAutoConsoleVariable<bool> CVarSpawnBots(TEXT("su.SpawnBots"), true,TEXT("Enable spawning of bots via timer."), ECVF_Cheat);

ASGameModeBase::ASGameModeBase()
{
	SpawnTimerInterval = 2.0f;
	MaxBotCount = 10.0f;

	DesiredPowerupCount = 10;
	RequiredPowerupDistance = 2000;

	PlayerStateClass = ASPlayerState::StaticClass();
}

void ASGameModeBase::StartPlay()
{
	Super::StartPlay();

	// 只有在数组中添加了开局时生成的拾取物类时才会执行这一段逻辑
	if (ensure(PowerupClasses.Num() > 0))
	{
		// 使用EQS找到生成拾取物的位置
		UEnvQueryInstanceBlueprintWrapper* QueryInstance = UEnvQueryManager::RunEQSQuery(this, PowerupSpawnQuery, this, EEnvQueryRunMode::AllMatching, nullptr);
		if (ensure(QueryInstance))
		{
			QueryInstance->GetOnQueryFinishedEvent().AddDynamic(this, &ASGameModeBase::OnPowerupSpawnQueryCompleted);
		}
	}

	GetWorldTimerManager().SetTimer(TimerHandle_SpawnBots, this, &ASGameModeBase::SpawnBotTimerElapsed,
	                                SpawnTimerInterval, true);
}

void ASGameModeBase::KillAll()
{
	for (TActorIterator<ASAICharacter> It(GetWorld()); It; ++It)
	{
		ASAICharacter* Bot = *It;

		USAttributeComponent* AttributeComp = USAttributeComponent::GetAttributes(Bot);
		if (ensure(AttributeComp) && AttributeComp->IsAlive())
		{
			AttributeComp->Kill(this); // @fixme: pass in player? for kill credit
		}
	}
}

void ASGameModeBase::OnActorKilled(AActor* VictimActor, AActor* Killer)
{
	// 玩家杀死玩家
	ASCharacter* Player = Cast<ASCharacter>(VictimActor);
	if (Player)
	{
		ASCharacter* KillerPlayer = Cast<ASCharacter>(Killer);
		// @Fixup：防止第三方物品或AI杀死AI触发奖励
		if (KillerPlayer)
		{
			ASPlayerState* KillerState = KillerPlayer->GetPlayerState<ASPlayerState>();
			if (KillerState)
			{
				// @todo: 解决硬编码
				KillerState->ApplyCreditsChange(50.0f);
			}
		}


		FTimerHandle TimerHandle_RespawnDelay;

		FTimerDelegate Delegate;
		Delegate.BindUFunction(this, "RespawnPlayerElapsed", Player->GetController());

		float RespawnDelay = 2.0f;
		GetWorldTimerManager().SetTimer(TimerHandle_RespawnDelay, Delegate, RespawnDelay, false);
	}

	// 玩家杀死AI
	ASAICharacter* AICharacter = Cast<ASAICharacter>(VictimActor);
	if (AICharacter)
	{
		// @Fixup：防止第三方物品或AI杀死AI触发奖励
		ASCharacter* KillerPlayer = Cast<ASCharacter>(Killer);
		if (KillerPlayer)
		{
			ASPlayerState* KillerState = KillerPlayer->GetPlayerState<ASPlayerState>();
			if (KillerState)
			{
				// @todo: 解决硬编码
				KillerState->ApplyCreditsChange(50.0f);
			}
		}
	}


	UE_LOG(LogTemp, Log, TEXT("OnActorKilled: Victim: %s, Killer: %s"), *GetNameSafe(VictimActor), *GetNameSafe(Killer));
}


void ASGameModeBase::SpawnBotTimerElapsed()
{
	if (!CVarSpawnBots.GetValueOnGameThread())
	{
		UE_LOG(LogTemp, Warning, TEXT("Bot spawning disable via cvar 'CVarSpawnBots'."));
		return;
	}

	UEnvQueryInstanceBlueprintWrapper* QueryInstance = UEnvQueryManager::RunEQSQuery(this, SpawnBotQuery, this, EEnvQueryRunMode::RandomBest5Pct, nullptr);

	if (ensure(QueryInstance))
	{
		QueryInstance->GetOnQueryFinishedEvent().AddDynamic(this, &ASGameModeBase::OnBotSpawnQueryCompleted);
	}
}

void ASGameModeBase::OnBotSpawnQueryCompleted(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus)
{
	if (QueryStatus != EEnvQueryStatus::Success)
	{
		UE_LOG(LogTemp, Warning, TEXT("Spawn bot EQS Query Failed!"));
		return;
	}

	int32 NrOfAliveBots = 0; // 统计游戏世界中的AI Bot数量
	for (TActorIterator<ASAICharacter> It(GetWorld()); It; ++It)
	{
		ASAICharacter* Bot = *It;

		// USAttributeComponent* AttributeComp = Cast<USAttributeComponent>(Bot->GetComponentByClass(USAttributeComponent::StaticClass()));
		// 由静态函数替代
		USAttributeComponent* AttributeComp = USAttributeComponent::GetAttributes(Bot);

		if (AttributeComp && AttributeComp->IsAlive())
		{
			NrOfAliveBots++;
		}
	}

	UE_LOG(LogTemp, Log, TEXT("Found %i alive bots."), NrOfAliveBots);

	// MaxBotCount = 10.0f;
	if (DifficultyCurve)
	{
		// 通过难度曲线设置AI Bot的上限
		MaxBotCount = DifficultyCurve->GetFloatValue(GetWorld()->TimeSeconds); // 随时间的曲线，根据时间（X轴）取值
	}

	if (NrOfAliveBots >= MaxBotCount)
	{
		UE_LOG(LogTemp, Log, TEXT("At maximum bot capacity.Skipping bot spawn."));
		return;
	}

	TArray<FVector> Locations = QueryInstance->GetResultsAsLocations();

	if (Locations.IsValidIndex(0))
	{
		GetWorld()->SpawnActor<AActor>(MinionClass, Locations[0], FRotator::ZeroRotator);

		DrawDebugSphere(GetWorld(), Locations[0], 50.0f, 20, FColor::Blue, false, 60.0f);
	}
}

void ASGameModeBase::OnPowerupSpawnQueryCompleted(UEnvQueryInstanceBlueprintWrapper* QueryInstance, EEnvQueryStatus::Type QueryStatus)
{
	if (QueryStatus != EEnvQueryStatus::Success)
	{
		UE_LOG(LogTemp, Warning, TEXT("Spawn bot EQS Query Failed!"));
		return;
	}

	TArray<FVector> Locations = QueryInstance->GetResultsAsLocations();

	// 把之前使用过的位置储存起来，这样我们可以更快地计算出距离
	TArray<FVector> UsedLocations;

	int32 SpawnCounter = 0;
	// 如果我们已经生成了足够多的拾取物或者没有更多的可以使用的生成点时结束
	while (SpawnCounter < DesiredPowerupCount && Locations.Num() > 0)
	{
		// 从剩下的点中随机选出一个
		int32 RandomLocationIndex = FMath::RandRange(0, Locations.Num() - 1);

		FVector PickedLocation = Locations[RandomLocationIndex];
		// 将选出的点删除避免重复使用
		Locations.RemoveAt(RandomLocationIndex);

		// 检查最小距离
		bool bValidLocation = true;
		for (FVector OtherLocation : UsedLocations)
		{
			float DistanceTo = (PickedLocation - OtherLocation).Size();

			if (DistanceTo < RequiredPowerupDistance)
			{
				// 可以选择使用Debug信息显示出因为距离而被筛选掉的点
				//DrawDebugSphere(GetWorld(), PickedLocation, 50.0f, 20, FColor::Red, false, 10.0f);

				// 点之间距离太近，挑选其他的
				bValidLocation = false;
				break;
			}
		}

		// 没有通过距离测试
		if (!bValidLocation)
		{
			continue;
		}

		// 随机挑选一个拾取物类
		int32 RandomClassIndex = FMath::RandRange(0, PowerupClasses.Num() - 1);
		TSubclassOf<AActor> RandomPowerupClass = PowerupClasses[RandomClassIndex];

		GetWorld()->SpawnActor<AActor>(RandomPowerupClass, PickedLocation, FRotator::ZeroRotator);

		// 将使用过的点储存起来用于距离测试
		UsedLocations.Add(PickedLocation);
		SpawnCounter++;
	}
}

void ASGameModeBase::RespawnPlayerElapsed(AController* Controller)
{
	if (ensure(Controller))
	{
		// 断开与死亡角色的所有关系
		Controller->UnPossess();

		// 重新启动控制器，取消在玩家死亡时禁用玩家输入
		RestartPlayer(Controller);
	}
}
