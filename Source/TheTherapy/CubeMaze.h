// Copyright (c) 2022 Wichit & Mika (name TBD)

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include <array>

#include "CubeMaze.generated.h"

UCLASS()
class THETHERAPY_API ACubeMaze : public AActor
{
  GENERATED_BODY()

public:
  // Sets default values for this actor's properties
  ACubeMaze();

  UPROPERTY(EditAnywhere)
  class USceneComponent *root;

  UPROPERTY(EditAnywhere)
  int sz = 8;

  UPROPERTY(EditAnywhere)
  TArray<class AMaze *> sides;

  UPROPERTY(EditAnywhere)
  class UBoxComponent *exitCollider0;

  UPROPERTY(EditAnywhere)
  class UBoxComponent *exitCollider1;

  UPROPERTY(EditAnywhere)
  class UBoxComponent *exitCollider2;

  UPROPERTY(EditAnywhere)
  class UBoxComponent *exitCollider3;

  UPROPERTY(EditAnywhere)
  class UBoxComponent *exitCollider4;

  UPROPERTY(EditAnywhere)
  class UBoxComponent *exitCollider5;

  UPROPERTY(EditAnywhere)
  FName nextLevel;

private:
  auto BeginPlay() -> void final;
  auto OnConstruction(const FTransform &Transform) -> void final;
  auto Tick(float DeltaTime) -> void final;
  auto resetMobAndMazeRegen() -> void;
  auto updateSidesVisibility() -> void;

  FTransform initialTransform;

  std::array<class UBoxComponent *, 6> exitColliders;
  std::array<class AMobSpawner *, 6> mobSpawners;
  std::array<class AHeart *, 6> hearts;

  UFUNCTION()
  void onOverlap(UPrimitiveComponent *HitComponent,
                 AActor *OtherActor,
                 UPrimitiveComponent *OtherComp,
                 int n,
                 bool b,
                 const FHitResult &Hit);

  UFUNCTION()
  void onHeartDestroyed(AActor *heart);

  int state = -1;
  float angle = 0.f;
  class USoundCue *mazeRegenSnd;
  auto resetNextRegen() -> void;
  auto getTime() const -> float;
  float nextRegen = 0.f;
  class USoundCue *cubeRotationSnd;
  class USoundCue *winSnd;
};
