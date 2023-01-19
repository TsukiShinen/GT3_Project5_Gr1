// Copyright Epic Games, Inc. All Rights Reserved.

#include "GT3_Project5_Gr1Character.h"

#include <Windows.Data.Text.h>

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"


//////////////////////////////////////////////////////////////////////////
// AGT3_Project5_Gr1Character

AGT3_Project5_Gr1Character::AGT3_Project5_Gr1Character()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = false; // Character moves in the direction of input...	
	GetCharacterMovement()->bUseControllerDesiredRotation = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = MaxWalkSpeed;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller
	
	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm
	FollowCamera->SetRelativeLocation(FVector(0, 100, 0));

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

void AGT3_Project5_Gr1Character::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
	
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	Weapon = GetWorld()->SpawnActor<AWeapon>(WeaponType, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
	if (Weapon)
	{
		Weapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("Weapon_R"));
	}

	AnimInstance = GetMesh()->GetAnimInstance();
}

//////////////////////////////////////////////////////////////////////////
// Input

void AGT3_Project5_Gr1Character::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		//Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AGT3_Project5_Gr1Character::Move);

		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AGT3_Project5_Gr1Character::Look);
		
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Started, this, &AGT3_Project5_Gr1Character::Aim);
		EnhancedInputComponent->BindAction(ShootAction, ETriggerEvent::Started, this, &AGT3_Project5_Gr1Character::Shoot);
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Completed, this, &AGT3_Project5_Gr1Character::EndAim);
	}

}

void AGT3_Project5_Gr1Character::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AGT3_Project5_Gr1Character::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AGT3_Project5_Gr1Character::Aim(const FInputActionValue& Value)
{
	bIsAiming = true;
	FollowCamera->SetRelativeLocation(FVector(0, 50, 50));
	CameraBoom->TargetArmLength = 70.0f;

	if (!bIsShooting)
	{
		AnimInstance->Montage_Play(ShootMontage);
		GetCharacterMovement()->MaxWalkSpeed = MaxWalkSpeedWhileAiming;
	}
}

void AGT3_Project5_Gr1Character::Shoot(const FInputActionValue& Value)
{
	if (bIsShooting) return;
	
	bIsShooting = true;

	AnimInstance->Montage_Play(ShootMontage);
	AnimInstance->Montage_JumpToSection(TEXT("Shoot"), ShootMontage);

	FOnMontageBlendingOutStarted OnMontageBlendingOutStarted;
	OnMontageBlendingOutStarted.BindUFunction(this, "EndShoot");
	AnimInstance->Montage_SetBlendingOutDelegate(OnMontageBlendingOutStarted);

	FVector start = FollowCamera->GetComponentLocation();
	FVector dir = FollowCamera->GetForwardVector();
	FVector end  = start + (dir * 100000);
	FHitResult hit;
	
	bool bIsActorHit = GetWorld()->LineTraceSingleByChannel(hit, start, end, ECC_Pawn, FCollisionQueryParams(), FCollisionResponseParams());
	if (bIsActorHit && hit.GetActor())
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, hit.GetActor()->GetName());
		Weapon->Shoot(hit.ImpactPoint);
	} else
	{
		Weapon->Shoot(end);
	}
}

void AGT3_Project5_Gr1Character::EndAim(const FInputActionValue& Value)
{
	bIsAiming = false;

	if (!bIsShooting)
	{
		AnimInstance->Montage_Stop(.5f, ShootMontage);
	}
	GetCharacterMovement()->MaxWalkSpeed = MaxWalkSpeed;
	FollowCamera->SetRelativeLocation(FVector(0, 100, 0));
	CameraBoom->TargetArmLength = 300.0f;
}

void AGT3_Project5_Gr1Character::EndShoot()
{
	bIsShooting = false;
	
	if (bIsAiming)
	{
		FollowCamera->SetRelativeLocation(FVector(0, 50, 50));
		CameraBoom->TargetArmLength = 70.0f;
		AnimInstance->Montage_Play(ShootMontage);
		GetCharacterMovement()->MaxWalkSpeed = MaxWalkSpeedWhileAiming;
	}
}




