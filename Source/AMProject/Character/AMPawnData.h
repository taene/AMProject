// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "AMPawnData.generated.h"

/**
 * 
 */
UCLASS()
class AMPROJECT_API UAMPawnData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UAMPawnData(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
};
