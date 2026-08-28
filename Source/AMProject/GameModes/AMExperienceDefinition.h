// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "AMExperienceDefinition.generated.h"

class UAMPawnData;
/**
 * 
 */
UCLASS()
class AMPROJECT_API UAMExperienceDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UAMExperienceDefinition(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UPROPERTY(EditDefaultsOnly, Category = Gameplay)
	TObjectPtr<UAMPawnData> DefaultPawnData;

	UPROPERTY(EditDefaultsOnly, Category = Gameplay)
	TArray<FString> GameFeaturesToEnable;
};
