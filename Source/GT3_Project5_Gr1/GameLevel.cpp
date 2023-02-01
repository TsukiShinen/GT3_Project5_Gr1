// Fill out your copyright notice in the Description page of Project Settings.


#include "GameLevel.h"

#include "Blueprint/UserWidget.h"

void AGameLevel::BeginPlay()
{
	Super::BeginPlay();
	CreateWidget<UUserWidget>(GetWorld(), Hud)->AddToViewport();

	CurrentZombieSpawned = 0;

	for (const auto Spawner : LstSpawner)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Red, TEXT("SPAWNER !"));
		Spawner->OnSpawn.AddDynamic(this, &AGameLevel::OnSpawn);
	}
}

void AGameLevel::OnSpawn()
{
	CurrentZombieSpawned ++;

	GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Red, FString::FromInt(CurrentZombieSpawned));
	if (CurrentZombieSpawned >= ZombiesPerPhase[0])
	{
		for (const auto Spawner : LstSpawner)
		{
			Spawner->SetActive(false);
		}
	}
}
