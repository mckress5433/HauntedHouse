// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/GeneratedCppIncludes.h"
#include "HauntedHouse/HauntedHouseGameMode.h"
#ifdef _MSC_VER
#pragma warning (push)
#pragma warning (disable : 4883)
#endif
PRAGMA_DISABLE_DEPRECATION_WARNINGS
void EmptyLinkFunctionForGeneratedCodeHauntedHouseGameMode() {}
// Cross Module References
	HAUNTEDHOUSE_API UClass* Z_Construct_UClass_AHauntedHouseGameMode_NoRegister();
	HAUNTEDHOUSE_API UClass* Z_Construct_UClass_AHauntedHouseGameMode();
	ENGINE_API UClass* Z_Construct_UClass_AGameModeBase();
	UPackage* Z_Construct_UPackage__Script_HauntedHouse();
// End Cross Module References
	void AHauntedHouseGameMode::StaticRegisterNativesAHauntedHouseGameMode()
	{
	}
	UClass* Z_Construct_UClass_AHauntedHouseGameMode_NoRegister()
	{
		return AHauntedHouseGameMode::StaticClass();
	}
	struct Z_Construct_UClass_AHauntedHouseGameMode_Statics
	{
		static UObject* (*const DependentSingletons[])();
#if WITH_METADATA
		static const UE4CodeGen_Private::FMetaDataPairParam Class_MetaDataParams[];
#endif
		static const FCppClassTypeInfoStatic StaticCppClassTypeInfo;
		static const UE4CodeGen_Private::FClassParams ClassParams;
	};
	UObject* (*const Z_Construct_UClass_AHauntedHouseGameMode_Statics::DependentSingletons[])() = {
		(UObject* (*)())Z_Construct_UClass_AGameModeBase,
		(UObject* (*)())Z_Construct_UPackage__Script_HauntedHouse,
	};
#if WITH_METADATA
	const UE4CodeGen_Private::FMetaDataPairParam Z_Construct_UClass_AHauntedHouseGameMode_Statics::Class_MetaDataParams[] = {
		{ "HideCategories", "Info Rendering MovementReplication Replication Actor Input Movement Collision Rendering Utilities|Transformation" },
		{ "IncludePath", "HauntedHouseGameMode.h" },
		{ "ModuleRelativePath", "HauntedHouseGameMode.h" },
		{ "ShowCategories", "Input|MouseInput Input|TouchInput" },
	};
#endif
	const FCppClassTypeInfoStatic Z_Construct_UClass_AHauntedHouseGameMode_Statics::StaticCppClassTypeInfo = {
		TCppClassTypeTraits<AHauntedHouseGameMode>::IsAbstract,
	};
	const UE4CodeGen_Private::FClassParams Z_Construct_UClass_AHauntedHouseGameMode_Statics::ClassParams = {
		&AHauntedHouseGameMode::StaticClass,
		"Game",
		&StaticCppClassTypeInfo,
		DependentSingletons,
		nullptr,
		nullptr,
		nullptr,
		UE_ARRAY_COUNT(DependentSingletons),
		0,
		0,
		0,
		0x008802ACu,
		METADATA_PARAMS(Z_Construct_UClass_AHauntedHouseGameMode_Statics::Class_MetaDataParams, UE_ARRAY_COUNT(Z_Construct_UClass_AHauntedHouseGameMode_Statics::Class_MetaDataParams))
	};
	UClass* Z_Construct_UClass_AHauntedHouseGameMode()
	{
		static UClass* OuterClass = nullptr;
		if (!OuterClass)
		{
			UE4CodeGen_Private::ConstructUClass(OuterClass, Z_Construct_UClass_AHauntedHouseGameMode_Statics::ClassParams);
		}
		return OuterClass;
	}
	IMPLEMENT_CLASS(AHauntedHouseGameMode, 3012500519);
	template<> HAUNTEDHOUSE_API UClass* StaticClass<AHauntedHouseGameMode>()
	{
		return AHauntedHouseGameMode::StaticClass();
	}
	static FCompiledInDefer Z_CompiledInDefer_UClass_AHauntedHouseGameMode(Z_Construct_UClass_AHauntedHouseGameMode, &AHauntedHouseGameMode::StaticClass, TEXT("/Script/HauntedHouse"), TEXT("AHauntedHouseGameMode"), false, nullptr, nullptr, nullptr);
	DEFINE_VTABLE_PTR_HELPER_CTOR(AHauntedHouseGameMode);
PRAGMA_ENABLE_DEPRECATION_WARNINGS
#ifdef _MSC_VER
#pragma warning (pop)
#endif
