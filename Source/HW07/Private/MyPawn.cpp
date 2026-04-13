//MyPawn.cpp

#include "MyPawn.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"



AMyPawn::AMyPawn()
{ 
	PrimaryActorTick.bCanEverTick = false;

	CapsuleComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComp"));
	RootComponent = CapsuleComp;

	SkeletalMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComp"));
	SkeletalMeshComp->SetupAttachment(CapsuleComp);

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	SpringArmComp->SetupAttachment(CapsuleComp);
	SpringArmComp->TargetArmLength = 300.0f;
	SpringArmComp->bUsePawnControlRotation = false;

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(SpringArmComp);
	CameraComp->bUsePawnControlRotation = false;

	CapsuleComp->SetSimulatePhysics(false);
	SkeletalMeshComp->SetSimulatePhysics(false);

	MoveSpeed = 300.0f;
	TurnSpeed = 100.0f;
}

void AMyPawn::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* LocalPlayer = Cast<APlayerController>(GetController());
	if (LocalPlayer)
	{
		UEnhancedInputLocalPlayerSubsystem* Subsystem =
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer->GetLocalPlayer());

		if (Subsystem && InputMappingContext)
		{
			Subsystem->AddMappingContext(InputMappingContext, 0);
		}
	}
	
}

void AMyPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMyPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);

	if (EnhancedInputComponent)
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMyPawn::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AMyPawn::Look);
	}

}

void AMyPawn::Move(const FInputActionValue& Value)
{
	FVector2D MoveValue = Value.Get<FVector2D>();

	FVector DeltaLocation = FVector::ZeroVector;
	DeltaLocation += GetActorForwardVector() * MoveValue.X * MoveSpeed * GetWorld()->GetDeltaSeconds();
	DeltaLocation += GetActorRightVector() * MoveValue.Y * MoveSpeed * GetWorld()->GetDeltaSeconds();

	AddActorWorldOffset(DeltaLocation, true);
}

void AMyPawn::Look(const FInputActionValue& Value)
{
	FVector2D LookValue = Value.Get<FVector2D>();

	FRotator CurrentRotation = GetActorRotation();
	CurrentRotation.Yaw += LookValue.X * TurnSpeed * GetWorld()->GetDeltaSeconds();

	SetActorRotation(CurrentRotation);

	FRotator SpringArmRotation = SpringArmComp->GetRelativeRotation();
	SpringArmRotation.Pitch = FMath::Clamp(
		SpringArmRotation.Pitch + LookValue.Y * TurnSpeed * GetWorld()->GetDeltaSeconds(),
		-80.0f,
		80.0f
	);

	SpringArmComp->SetRelativeRotation(SpringArmRotation);
}