// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonGameViewportClient.h"
#include "UWorldCommonGameViewportClient.generated.h"

/**
 * 
 */
UCLASS()
class GAMEPROJECT_API UUWorldCommonGameViewportClient : public UCommonGameViewportClient
{
	GENERATED_BODY()
public:

	void ChangeRenderWorld(UWorld* NewWorld) { World = NewWorld; }

};
