// Copyright Epic Games, Inc. All Rights Reserved.

#include "SamplePluginBPLibrary.h"

USamplePluginBPLibrary::USamplePluginBPLibrary(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{

}

float USamplePluginBPLibrary::CalculateSum(float A, float B)
{
	return CoreSum(A, B);
}

