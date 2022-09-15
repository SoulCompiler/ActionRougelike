// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
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
	bool ApplyCreditsChange(float Delta);

protected:
	UPROPERTY(VisibleAnywhere, Category = "Credits")
	float Credits;
	
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnCreditsChanged OnCreditsChanged;
};
