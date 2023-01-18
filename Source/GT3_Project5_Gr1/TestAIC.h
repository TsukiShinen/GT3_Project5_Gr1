#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include <Kismet/GameplayStatics.h>
#include "TestAIC.generated.h"

UCLASS()
class GT3_PROJECT5_GR1_API ATestAIC : public AAIController
{
	GENERATED_BODY()
	
public:
	ATestAIC();

	EPathFollowingRequestResult::Type MoveToActor();

protected:
	virtual void BeginPlay();
};