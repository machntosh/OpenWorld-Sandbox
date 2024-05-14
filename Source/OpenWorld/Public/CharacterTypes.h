#pragma once

UENUM(BlueprintType)
enum class ECharacterState : uint8
{
	ECS_Unequipped UMETA(DisplayName = "Unequipped"),
	ECS_EquippedOneHandedWeapon UMETA(DisplayName = "Equipped One-Handed Weapon"),
	ECS_EquippedTwoHandedWeapon UMETA(DisplayName = "Equipped Two-Handed Weapon"),
	
};

UENUM(BlueprintType)
enum class EActionState : uint8 
{
	EAS_Unoccupied UMETA(DisplayName = "Unoccupied"),
	EAS_HitReaction UMETA(DisplayName = "Hit Reacting"),
	EAS_Attacking UMETA(DisplayName = "Attacking"),
	EAS_Equipping UMETA(DisplayNamde = "Equipping"),
	EAS_Dodging UMETA(DisplayName = "Dodging"),
	EAS_Dead UMETA(DisplayName = "Dead")
};

UENUM(BlueprintType)
enum EDeathPose 
{
	EDP_DeathLeft UMETA(DisplayName = "Death Left"),
	EDP_DeathRight UMETA(DisplayName = "Death Right"),
	EDP_DeathForward UMETA(DisplayName = "Death Forward"),
	EDP_DeathBackward UMETA(DisplayName = "Death Backward"),

	EDP_MAX UMETA(DisplayName = "DefaultMAX")
};
UENUM(BlueprintType)
enum class EEnemyState : uint8
{
	EES_NoState UMETA(DisplayName = "NoState" ),
	
	EES_Dead UMETA(DisplayName = "Dead" ),
	EES_Patrolling UMETA(DisplayName = "Patrolling" ),
	EES_Chasing UMETA(DisplayName = "Chasing" ),
	EES_Attacking UMETA(DisplayName = "Attacking" ),
	EES_Engaged UMETA(DisplayName = "Engaged" )
};
