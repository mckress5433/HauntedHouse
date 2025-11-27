// Fill out your copyright notice in the Description page of Project Settings.


#include "InGamePlayerState.h"
#include "OnlineSubsystem.h"
#include "HauntedHouse/HauntedHouse.h"
#include "HauntedHouse/Character/InGameCharacter.h"
#include "HauntedHouse/Game/SaveSystem/SaveGameSubsystem.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "HauntedHouse/GameplayEffects/InstantOverrideAllEffect.h"
#include "HauntedHouse/Global/GlobalFunctionLibrary.h"
#include "Net/UnrealNetwork.h"

AInGamePlayerState::AInGamePlayerState()
{
	bReplicates = true;

	AbilitySystemComponent = CreateDefaultSubobject<UCharacterAbilitySystemComponent>(FName("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	CreateDefaultSubobject<UCharacterAttributeSet>(FName("AttributeSet"));
}

void AInGamePlayerState::InitializeAttributes(const FCharacterAttributeData& CharacterAttributeData) const
{
	if (GetLocalRole() == ROLE_Authority)
	{
		const UInstantOverrideAllEffect* InitializeAttributesEffect = NewObject<UInstantOverrideAllEffect>(GetTransientPackage(), FName(TEXT("InstanteOverrideAllEffect")));
		const TSubclassOf<UInstantOverrideAllEffect> DynamicGameplayEffect = InitializeAttributesEffect->GetClass();
		
		FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
		EffectContext.AddSourceObject(this);
		
		const FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(DynamicGameplayEffect, 1.f, EffectContext);
		SpecHandle.Data->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(FName("SetByCaller.Health")), CharacterAttributeData.MaxHealth);
		SpecHandle.Data->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(FName("SetByCaller.MaxHealth")), CharacterAttributeData.MaxHealth);
		SpecHandle.Data->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(FName("SetByCaller.Stamina")), CharacterAttributeData.MaxStamina);
		SpecHandle.Data->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(FName("SetByCaller.MaxStamina")), CharacterAttributeData.MaxStamina);
		SpecHandle.Data->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(FName("SetByCaller.StaminaRegenRate")), CharacterAttributeData.StaminaRegenRate);
		SpecHandle.Data->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(FName("SetByCaller.Strength")), CharacterAttributeData.BaseStrength);
		SpecHandle.Data->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(FName("SetByCaller.Speed")), CharacterAttributeData.BaseSpeed);
		SpecHandle.Data->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(FName("SetByCaller.Intelligence")), CharacterAttributeData.BaseIntelligence);
		SpecHandle.Data->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(FName("SetByCaller.Sanity")), CharacterAttributeData.BaseSanity);
		
		AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	}
}

void AInGamePlayerState::UpdateCharacterInfoAndMeshes(const FPlayersCharacterInfo& PlayersCharacterInfo)
{
	if (GetLocalRole() == ROLE_Authority && AbilitySystemComponent != nullptr)
	{
		// Update the CharacterInfo struct and call modify to mark the struct as dirty to force a replication update
		CharacterInfo = PlayersCharacterInfo;
		Modify();
	
		const FCharacterAttributeData CharacterAttributeData = CharacterInfo.CharacterAttributeData;
		InitializeAttributes(CharacterAttributeData);
	
		if(auto character = Cast<AInGameCharacter>(GetPawn()); character != nullptr)
		{
			character->UpdateMeshes(CharacterInfo.CharacterMeshData, CharacterInfo.CharacterColor);
		}
	}
}

void AInGamePlayerState::Server_UpdatePlayerData_Implementation(FSessionSaveStruct SessionSaveStruct)
{
	UpdateCharacterInfoAndMeshes(SessionSaveStruct.CharacterInfo);
}

void AInGamePlayerState::PrintSessionData() const
{
	UE_LOG(LogAttributes, Log, TEXT("Session Data..."));
	UE_LOG(LogAttributes, Log, TEXT("    Character color: %s"), *CharacterInfo.CharacterColor.ToString());
	UE_LOG(LogAttributes, Log, TEXT("    Third Person Mesh: %s"), *CharacterInfo.CharacterMeshData.ThirdPersonMesh->GetFullName());
	PrintAttributeData(CharacterInfo.CharacterAttributeData);
}


void AInGamePlayerState::HealthChanged(const FOnAttributeChangeData& Data)
{
	if(Data.Attribute == UCharacterAttributeSet::GetHealthAttribute())
	{
		OnHealthChangedDelegate.Broadcast(static_cast<float>(Data.NewValue));
	}
}

void AInGamePlayerState::MaxHealthChanged(const FOnAttributeChangeData& Data)
{
	if(Data.Attribute == UCharacterAttributeSet::GetMaxHealthAttribute())
	{
		OnMaxHealthChangedDelegate.Broadcast(static_cast<float>(Data.NewValue));
	}
}

void AInGamePlayerState::StaminaChanged(const FOnAttributeChangeData& Data)
{
	OnStaminaChangedDelegate.Broadcast(static_cast<float>(Data.NewValue));
}

void AInGamePlayerState::MaxStaminaChanged(const FOnAttributeChangeData& Data)
{
	OnMaxStaminaChangedDelegate.Broadcast(static_cast<float>(Data.NewValue));
}

void AInGamePlayerState::StaminaRegenRateChanged(const FOnAttributeChangeData& Data)
{
	OnStaminaRegenRateChangedDelegate.Broadcast(static_cast<float>(Data.NewValue));
}

void AInGamePlayerState::StrengthChanged(const FOnAttributeChangeData& Data)
{
	OnStrengthChangedDelegate.Broadcast(static_cast<float>(Data.NewValue));
}

void AInGamePlayerState::SpeedChanged(const FOnAttributeChangeData& Data)
{
	OnSpeedChangedDelegate.Broadcast(static_cast<float>(Data.NewValue));
}

void AInGamePlayerState::IntelligenceChanged(const FOnAttributeChangeData& Data)
{
	OnIntelligenceChangedDelegate.Broadcast(static_cast<float>(Data.NewValue));
}

void AInGamePlayerState::SanityChanged(const FOnAttributeChangeData& Data)
{
	OnSanityChangedDelegate.Broadcast(static_cast<float>(Data.NewValue));
}

void AInGamePlayerState::BeginPlay()
{
	Super::BeginPlay();

	if(AbilitySystemComponent != nullptr)
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UCharacterAttributeSet::GetHealthAttribute()).AddUObject(this, &ThisClass::HealthChanged);
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UCharacterAttributeSet::GetMaxHealthAttribute()).AddUObject(this, &ThisClass::MaxHealthChanged);
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UCharacterAttributeSet::GetStaminaAttribute()).AddUObject(this, &ThisClass::HealthChanged);
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UCharacterAttributeSet::GetMaxStaminaAttribute()).AddUObject(this, &ThisClass::HealthChanged);
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UCharacterAttributeSet::GetStaminaRegenRateAttribute()).AddUObject(this, &ThisClass::HealthChanged);
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UCharacterAttributeSet::GetStrengthAttribute()).AddUObject(this, &ThisClass::HealthChanged);
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UCharacterAttributeSet::GetSpeedAttribute()).AddUObject(this, &ThisClass::HealthChanged);
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UCharacterAttributeSet::GetIntelligenceAttribute()).AddUObject(this, &ThisClass::HealthChanged);
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UCharacterAttributeSet::GetSanityAttribute()).AddUObject(this, &ThisClass::HealthChanged);
	}

	if (HasAuthority() && AbilitySystemComponent != nullptr)
	{
		if(APlayerController* PC = GetPlayerController(); PC != nullptr)
		{
			if (PC->IsLocalController())
			{
				const FCharacterAttributeData CharacterAttributeData = CharacterInfo.CharacterAttributeData;
				InitializeAttributes(CharacterAttributeData);
			}
		}

		AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(BaseAbilities.ToggleOptionsAbility, 1, INDEX_NONE));
	}

	// Get the Online Subsystem
	IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
	if (OnlineSubsystem)
	{
		IOnlineIdentityPtr IdentityInterface = OnlineSubsystem->GetIdentityInterface();
		if (IdentityInterface.IsValid())
		{
			// Get the UniqueNetId of the player
			TSharedPtr<const FUniqueNetId> UniqueNetId = IdentityInterface->GetUniquePlayerId(GetLocalRole());
			if (UniqueNetId.IsValid())
			{
				// Get the Steam username
				PlayerUsername = IdentityInterface->GetPlayerNickname(*UniqueNetId);
			}
		}
	}
}

void AInGamePlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ThisClass, CharacterInfo);
	DOREPLIFETIME(ThisClass, AbilitySystemComponent);
	DOREPLIFETIME(ThisClass, AttributeSet);
}

FCharacterAttributeData AInGamePlayerState::GetCharacterAttributeData() const
{
	FCharacterAttributeData attributeData;

	attributeData.BaseIntelligence = AttributeSet->Intelligence.GetCurrentValue();
	attributeData.BaseSanity = AttributeSet->Sanity.GetCurrentValue();
	attributeData.BaseSpeed = AttributeSet->Speed.GetCurrentValue();
	attributeData.BaseStrength = AttributeSet->Strength.GetCurrentValue();
	attributeData.MaxHealth = AttributeSet->MaxHealth.GetCurrentValue();
	attributeData.MaxStamina = AttributeSet->MaxStamina.GetCurrentValue();
	attributeData.StaminaRegenRate = AttributeSet->StaminaRegenRate.GetCurrentValue();
	
	return attributeData;
}

void AInGamePlayerState::PrintAttributeData(const FCharacterAttributeData& AttributeData)
{
	UE_LOG(LogAttributes, Log, TEXT("Character Attributes..."));
	UE_LOG(LogAttributes, Log, TEXT("    BaseIntelligence: %f"), AttributeData.BaseIntelligence);
	UE_LOG(LogAttributes, Log, TEXT("    BaseSanity: %f"), AttributeData.BaseSanity);
	UE_LOG(LogAttributes, Log, TEXT("    BaseSpeed: %f"), AttributeData.BaseSpeed);
	UE_LOG(LogAttributes, Log, TEXT("    BaseStrength: %f"), AttributeData.BaseStrength);
	UE_LOG(LogAttributes, Log, TEXT("    MaxStamina: %f"), AttributeData.MaxStamina);
	UE_LOG(LogAttributes, Log, TEXT("    StaminaRegenRate: %f"), AttributeData.StaminaRegenRate);
}

void AInGamePlayerState::OnRep_CharacterInfo() const
{
	if (GetLocalRole() != ROLE_Authority)
	{
		// Update the character meshes
		if(auto character = Cast<AInGameCharacter>(GetPawn()); character != nullptr)
		{
			character->UpdateMeshes(CharacterInfo.CharacterMeshData, CharacterInfo.CharacterColor);
		}
	}
}

UAbilitySystemComponent* AInGamePlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AInGamePlayerState::Client_LoadPlayerData_Implementation()
{
	UGameInstance* gameInstance = GetGameInstance();
	UWorld* world = GetWorld();
	if(gameInstance != nullptr && world != nullptr)
	{
		auto saveSystem = gameInstance->GetSubsystem<USaveGameSubsystem>();
		
		if (saveSystem != nullptr)
		{
			saveSystem->LoadOrCreateSessionSaveData([this](const FSessionSaveStruct& SessionData)
			{
				Server_UpdatePlayerData(SessionData);
				
				if (GlobalFunctionLibrary::GetSaveSystemDebugValue() != 0)
				{
					PrintSessionData();
				}
			});
		}
	}
}