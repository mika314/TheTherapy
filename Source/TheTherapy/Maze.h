// Copyright (c) 2022 Wichit & Mika (name TBD)

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Maze.generated.h"

UCLASS()
class THETHERAPY_API AMaze : public AActor
{
  GENERATED_BODY()

public:
  // Sets default values for this actor's properties
  AMaze();

  UPROPERTY(EditAnywhere)
  int sz = 8;

  UPROPERTY(EditAnywhere)
  int startCorner = 0;

  UPROPERTY(EditAnywhere)
  int endCorner = 1;

  UPROPERTY(EditAnywhere)
  class USceneComponent *root;

  UPROPERTY(EditAnywhere)
  class UInstancedStaticMeshComponent *cornerShort;

  UPROPERTY(EditAnywhere)
  class UInstancedStaticMeshComponent *cornerTall;

  UPROPERTY(EditAnywhere)
  class UInstancedStaticMeshComponent *wallShort;

  UPROPERTY(EditAnywhere)
  class UInstancedStaticMeshComponent *wallTall;

  UPROPERTY(EditAnywhere)
  class UInstancedStaticMeshComponent *floor;

private:
  // Called when the game starts or when spawned
  auto BeginPlay() -> void final;
  auto OnConstruction(const FTransform &Transform) -> void final;

  // Called every frame
  auto Tick(float DeltaTime) -> void final;
};