// Copyright Epic Games, Inc. All Rights Reserved.

#include "TheTherapyCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/DecalComponent.h"
#include "Engine/World.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "HudUi.h"
#include "Materials/Material.h"
#include "Maze.h"
#include "Mob.h"
#include "PrjHud.h"
#include "UObject/ConstructorHelpers.h"
#include <Components/PointLightComponent.h>
#include <GameFramework/PlayerController.h>
#include <Kismet/GameplayStatics.h>
#include <QofL/abbr.h>
#include <QofL/check_ret.h>
#include <QofL/log.h>
#include <QofL/obj_finder.h>
#include <Sound/SoundCue.h>

ATheTherapyCharacter::ATheTherapyCharacter()
  : light(CreateDefaultSubobject<UPointLightComponent>(TEXT("light"))),
    stepsSnd(OBJ_FINDER(SoundCue, "Footsteps", "Footsteps")),
    heartSnd(OBJ_FINDER(SoundCue, "SFX", "SND_Heart_Cue")),
    deathSnd(OBJ_FINDER(SoundCue, "SFX", "SND_Death_Cue"))
{
  // Set size for player capsule
  GetCapsuleComponent()->InitCapsuleSize(21.f, 96.0f);

  // Don't rotate character to camera direction
  bUseControllerRotationPitch = false;
  bUseControllerRotationYaw = false;
  bUseControllerRotationRoll = false;

  // Configure character movement
  GetCharacterMovement()->bOrientRotationToMovement = true; // Rotate character to moving direction
  GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
  GetCharacterMovement()->bConstrainToPlane = true;
  GetCharacterMovement()->bSnapToPlaneAtStart = true;

  // Create a camera boom...
  CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
  CameraBoom->SetupAttachment(RootComponent);
  CameraBoom->SetUsingAbsoluteRotation(true); // Don't want arm to rotate when character does
  CameraBoom->TargetArmLength = 1600.f;
  CameraBoom->SetRelativeRotation(FRotator(-60.f, 0.f, 0.f));
  CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level

  // Create a camera...
  TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
  TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
  TopDownCameraComponent->bUsePawnControlRotation = false; // Camera does not rotate relative to arm
  light->SetupAttachment(RootComponent);

  // Activate ticking in order to update the cursor every frame.
  PrimaryActorTick.bCanEverTick = true;
  PrimaryActorTick.bStartWithTickEnabled = true;
}

void ATheTherapyCharacter::Tick(float DeltaSeconds)
{
  Super::Tick(DeltaSeconds);
}

auto ATheTherapyCharacter::BeginPlay() -> void
{
  Super::BeginPlay();
  heartsCount = 0;
  m_isAlive = true;
  m_isWin = false;
  OnActorHit.AddDynamic(this, &ATheTherapyCharacter::onHit);
}

auto ATheTherapyCharacter::addHeart() -> void
{
  LOG("add heart");
  ++heartsCount;
  auto hud = Cast<APlayerController>(GetController())->GetHUD<APrjHud>();
  CHECK_RET(hud);
  auto hudUi = hud->hudUi;
  CHECK_RET(hudUi);
  hudUi->setHearts(heartsCount);
  UGameplayStatics::PlaySoundAtLocation(GetWorld(), heartSnd, getLoc(this));
}

auto ATheTherapyCharacter::getHeartsCount() -> int
{
  return heartsCount;
}

auto ATheTherapyCharacter::setDistanceToTheGoal(float val) -> void
{
  auto l = log(val) * .75f - 3.5f;
  light->SetLightColor(FLinearColor{l, 1.f - l, 0.f});
}

void ATheTherapyCharacter::footStep()
{
  UGameplayStatics::PlaySoundAtLocation(GetWorld(), stepsSnd, getLoc(this));
}

bool ATheTherapyCharacter::isAlive() const
{
  return m_isAlive;
}

void ATheTherapyCharacter::onHit(AActor *self, AActor *other, FVector imp, const FHitResult &hit)
{
  auto mob = Cast<AMob>(other);
  if (!mob)
    return;
  if (!m_isAlive)
    return;
  if (m_isWin)
    return;
  LOG("died");
  m_isAlive = false;
  UGameplayStatics::PlaySoundAtLocation(GetWorld(), deathSnd, getLoc(this));
}

auto ATheTherapyCharacter::won() -> void
{
  LOG("Level is cleared");
  m_isWin = true;
}

bool ATheTherapyCharacter::isWin() const
{
  return m_isWin;
}

auto ATheTherapyCharacter::getObscuringWalls() -> std::vector<AMaze *>
{
  TArray<struct FHitResult> OutHits;
  if (!GetWorld()->LineTraceMultiByChannel(OutHits,
                                           TopDownCameraComponent->GetComponentLocation(),
                                           GetMesh()->GetComponentLocation() + vec(0., 0., 90.),
                                           ECC_Camera))
    return {};
  std::vector<AMaze *> ret;
  for (auto &hit : OutHits)
  {
    if (!hit.Component.Get())
      continue;
    auto wall = Cast<AMaze>(hit.Component.Get()->GetOwner());
    if (!wall)
      continue;
    ret.push_back(wall);
  }
  return ret;
}
