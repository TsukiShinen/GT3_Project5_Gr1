// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Spawner.h"
#include "Engine/LevelScriptActor.h"
#include "GameLevel.generated.h"

/**
 * 
 */
UCLASS()
class GT3_PROJECT5_GR1_API AGameLevel : public ALevelScriptActor
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent);

protected:
	void DeactivateSpawner();
	UFUNCTION()
	void OnSpawn(AEnemySkeleton* EnemySpawned);
	void ActivateSpawner();
	void NextPhase();
	UFUNCTION()
	void OnEnemyDeath();
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> Hud;
	
	UPROPERTY(EditAnywhere)
	TArray<ASpawner*> LstSpawner;
	
	UPROPERTY(EditAnywhere)
	TArray<int> ZombiesPerPhase;
	
	UPROPERTY()
	int CurrentPhase;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* PauseAction;

	UPROPERTY()
	int CurrentZombieSpawned;
	
	UPROPERTY()
	int CurrentZombieAlive;

	UFUNCTION()
	void PauseGame();
};
