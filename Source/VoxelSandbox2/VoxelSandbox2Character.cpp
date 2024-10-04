// Copyright Epic Games, Inc. All Rights Reserved.

#include "VoxelSandbox2Character.h"
#include "Engine/LocalPlayer.h"
#include "Engine/World.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"

#include "TerrainTool.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// AVoxelSandbox2Character

AVoxelSandbox2Character::AVoxelSandbox2Character()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

void AVoxelSandbox2Character::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();
}

void AVoxelSandbox2Character::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


//////////////////////////////////////////////////////////////////////////
// Input

void AVoxelSandbox2Character::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {

		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AVoxelSandbox2Character::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AVoxelSandbox2Character::Look);

		// Bind left-click (dig) and right-click (fill)
		EnhancedInputComponent->BindAction(DigAction, ETriggerEvent::Triggered, this, &AVoxelSandbox2Character::PerformDigAction);
		EnhancedInputComponent->BindAction(FillAction, ETriggerEvent::Triggered, this, &AVoxelSandbox2Character::PerformFillAction);

		// Shape controls
		//EnhancedInputComponent->BindAction(ChangeShapeAction, ETriggerEvent::Triggered, this, &AVoxelSandbox2Character::ChangeShape);

		//// Size controls
		//EnhancedInputComponent->BindAction(ChangeSizeAction, ETriggerEvent::Triggered, this, &AVoxelSandbox2Character::ChangeSize);

	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}


void AVoxelSandbox2Character::Move(const FInputActionValue& Value)
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

void AVoxelSandbox2Character::Look(const FInputActionValue& Value)
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

//////////////////////////////////////////////////////////////////////////
// Tool Functions


void AVoxelSandbox2Character::EquipTool(TSubclassOf<ATerrainTool> NewToolClass) {
	if (NewToolClass) {
		// Destroy the previously equipped tool if necessary
		if (EquippedTool) {
			EquippedTool->Destroy();
		}

		// Spawn the new tool
		EquippedTool = GetWorld()->SpawnActor<ATerrainTool>(NewToolClass);

		// Attach the tool to the character's weapon bone
		if (EquippedTool) {
			// Replace "Weapon" with the exact name of your bone/socket
			EquippedTool->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, TEXT("Hand_r"));
			// Adjust its position and rotation
			EquippedTool->SetActorRelativeLocation(FVector(-77.0f, 106.0f, 41.0f)); // Adjust Z to lift it up
			EquippedTool->SetActorRelativeRotation(FRotator(0.0f, 82.0f, -114.0f)); // Rotate if needed
		}
	}
}


void AVoxelSandbox2Character::UnequipTool() {
	EquippedTool = nullptr;
}

void AVoxelSandbox2Character::PerformDigAction() {
	UE_LOG(LogTemp, Warning, TEXT("Perform Dig Action Started"));

	if (EquippedTool) {
		FHitResult HitResult;
		if (PerformRayTrace(HitResult)) {
			EquippedTool->DigTerrain(HitResult);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Dig PerformRayTrace found no hit result"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("No Equipped tool found!"));
	}
}

void AVoxelSandbox2Character::PerformFillAction() {
	UE_LOG(LogTemp, Warning, TEXT("Perform Fill Action Started"));

	if (EquippedTool) {
		FHitResult HitResult;
		if (PerformRayTrace(HitResult)) {
			EquippedTool->FillTerrain(HitResult);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Fill PerformRayTrace found no hit result"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("No Equipped tool found!"));
	}
}


//////////////////////////////////////////////////////////////////////////
// Utility Functions

bool AVoxelSandbox2Character::PerformRayTrace(FHitResult& OutHitResult) {
	UE_LOG(LogTemp, Error, TEXT("Performing RayTrace"));

	FVector CameraLocation;
	FRotator CameraRotation;

	if (APlayerController* PlayerController = Cast<APlayerController>(GetController())) {
		PlayerController->GetPlayerViewPoint(CameraLocation, CameraRotation);

		FVector StartLocation = CameraLocation;
		FVector EndLocation = StartLocation + (CameraRotation.Vector() * 1000);

		FCollisionQueryParams Params;
		Params.AddIgnoredActor(this);

		return GetWorld()->LineTraceSingleByChannel(OutHitResult, StartLocation, EndLocation, ECC_Visibility, Params);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("No Player Controller Found"));

	}

	return false;
}

