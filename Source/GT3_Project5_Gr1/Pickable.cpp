﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "Pickable.h"

#include "GT3_Project5_Gr1Character.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetMathLibrary.h"


// Sets default values for this component's properties
UPickable::UPickable()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &UPickable::OnOverlapBegin);
	
	Text = CreateDefaultSubobject<UTextRenderComponent>(TEXT("Text"));
}


// Called when the game starts
void UPickable::BeginPlay()
{
	Super::BeginPlay();

	if (Vfx)
	{
		UNiagaraFunctionLibrary::SpawnSystemAttached(Vfx, GetOwner()->GetRootComponent(), NAME_None, FVector(0.f),
		                                             FRotator(0.f), EAttachLocation::Type::KeepRelativeOffset, true);
	}
}


void UPickable::SetTextLookAtPlayer() const
{
	const auto TextLocation = Text->GetComponentLocation();
	const auto PlayerLocation = GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation();
	const auto NewRotation = UKismetMathLibrary::FindLookAtRotation(TextLocation, PlayerLocation);

	Text->SetWorldRotation(NewRotation);
}

// Called every frame
void UPickable::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!bIsActiveUpdate) { return; }
	Float(DeltaTime);
	SetTextLookAtPlayer();
}

void UPickable::AttachTo(USceneComponent* Component) const
{
	TriggerBox->SetupAttachment(Component);
	TriggerBox->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));
	
	Text->SetupAttachment(Component);
	Text->SetRelativeLocation(FVector(0, 0, 50));
}

void UPickable::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                               int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!bIsActiveUpdate) { return; }
	if (auto Player = Cast<AGT3_Project5_Gr1Character>(OtherActor))
	{
		OnPickUp.Broadcast(Player);
	}
}

void UPickable::Float(float DeltaTime) const
{
	FVector NewLocation = GetOwner()->GetActorLocation();
	FRotator NewRotation = GetOwner()->GetActorRotation();
	const float RunningTime = GetOwner()->GetGameTimeSinceCreation();
	const float DeltaHeight = (FMath::Sin(RunningTime + DeltaTime) - FMath::Sin(RunningTime));
	NewLocation.Z += DeltaHeight * 10.0f;
	const float DeltaRotation = DeltaTime * 80.0f;
	NewRotation.Yaw += DeltaRotation;
	GetOwner()->SetActorLocationAndRotation(NewLocation, NewRotation);
}
