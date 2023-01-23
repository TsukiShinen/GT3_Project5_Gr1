﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ArrowComponent.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

UCLASS()
class GT3_PROJECT5_GR1_API AWeapon : public AActor
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	UArrowComponent* SpawnBullet;
	
	UPROPERTY(EditAnywhere)
	USkeletalMeshComponent* VisualMesh;

	UPROPERTY(EditAnywhere)
	float Damage = 50;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<UDamageType> DamageType;

	UPROPERTY(EditAnywhere)
	bool bIsAuto;

public:
	// Sets default values for this actor's properties
	AWeapon();

	bool IsAuto() const { return bIsAuto; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UTexture* Image;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void Shoot(FVector End, AActor* Actor = nullptr);
};
