// Fill out your copyright notice in the Description page of Project Settings.


#include "InGamePlayerState.h"
#include "MVVMViewModelBase.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"
#include "HauntedHouse/Character/InGameCharacter.h"
#include "HauntedHouse/Game/HauntedHouseGameInstance.h"
#include "HauntedHouse/Game/LobbyGameState.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "HauntedHouse/GameplayEffects/InstantOverrideAllEffect.h"
#include "Net/UnrealNetwork.h"

AInGamePlayerState::AInGamePlayerState()
{
	bReplicates = true;
	
	AbilitySystemComponent = CreateDefaultSubobject<UCharacterAbilitySystemComponent>(FName("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	// AbilitySystemComponent needs to be updated at a high frequency.
	NetUpdateFrequency = 1000.0f;

	CreateDefaultSubobject<UCharacterAttributeSet>(FName("AttributeSet"));
	
}

void AInGamePlayerState::InitializeAttributes(const FCharacterAttributeData CharacterAttributeData) const
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

void AInGamePlayerState::UpdateCharacterInfoAndMeshes(UBaseCharacterDataAsset* DataAsset)
{
	if (GetLocalRole() == ROLE_Authority && AbilitySystemComponent != nullptr && CharacterDataAsset != nullptr)
	{
		const FCharacterAttributeData CharacterAttributeData = DataAsset->GetCharacterAttributeData();
		InitializeAttributes(CharacterAttributeData);
	}

	if(auto character = Cast<AInGameCharacter>(GetPawn()); character != nullptr)
	{
		character->UpdateMeshes(DataAsset->GetCharacterMeshData(), DataAsset->GetCharacterColor());
	}
}

void AInGamePlayerState::HandleCharacterSelection_Implementation(UBaseCharacterDataAsset* DataAsset)
{
	if(auto GS = GetWorld()->GetGameState<ALobbyGameState>(); GS != nullptr)
	{
		GS->UpdatePlayerCharactersInfos(this, DataAsset);
	}
}

void AInGamePlayerState::HealthChanged(const FOnAttributeChangeData& Data)
{
	if(Data.Attribute == UCharacterAttributeSet::GetHealthAttribute())
	{
		OnCurrentHealthChangedDelegate.Broadcast(static_cast<float>(Data.NewValue));
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
	OnCurrentStaminaChangedDelegate.Broadcast(static_cast<float>(Data.NewValue));
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
			if (PC->IsLocalController() && CharacterDataAsset != nullptr)
			{
				const FCharacterAttributeData CharacterAttributeData = CharacterDataAsset->GetCharacterAttributeData();
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
	
	DOREPLIFETIME(ThisClass, CharacterDataAsset);
}

UAbilitySystemComponent* AInGamePlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AInGamePlayerState::UpdateDefaultCharacterDataAsset(UBaseCharacterDataAsset* DataAsset)
{
	CharacterDataAsset = DataAsset;

	if(APlayerController* PC = GetPlayerController(); PC != nullptr)
	{
		if(HasAuthority() && !PC->IsLocalController() && AbilitySystemComponent != nullptr && CharacterDataAsset != nullptr)
		{
			const FCharacterAttributeData CharacterAttributeData = CharacterDataAsset->GetCharacterAttributeData();
			InitializeAttributes(CharacterAttributeData);
		}
	}
}