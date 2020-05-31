#pragma once

#include "Native API/Radio API.h"

namespace FrequencyOffsetConverter
{
	static const FrequencyType MinimumFrequency = 87500;
	static const FrequencyType MaximumFrequency = 108000;
	static const auto ScaleFactor = 0.4;

	double Convert(FrequencyType Frequency)
	{
		return (Frequency - MinimumFrequency) * ScaleFactor;
	}

	FrequencyType ConvertBack(double HorizontalOffset)
	{
		return static_cast<FrequencyType>((HorizontalOffset / ScaleFactor) + MinimumFrequency);
	}
}