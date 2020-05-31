#pragma once

#include "Native API/Radio API.h"
#include "FrequencyFormattingConverter.g.h"

namespace winrt::FMRadio::implementation
{
	struct FrequencyFormattingConverter : FrequencyFormattingConverterT<FrequencyFormattingConverter>
	{
		static hstring Convert(hstring);

		Windows::Foundation::IInspectable Convert(Windows::Foundation::IInspectable const &, Windows::UI::Xaml::Interop::TypeName const &, Windows::Foundation::IInspectable const &, hstring const &);
		Windows::Foundation::IInspectable ConvertBack(Windows::Foundation::IInspectable const &, Windows::UI::Xaml::Interop::TypeName const &, Windows::Foundation::IInspectable const &, hstring const &);
	};
}

namespace winrt::FMRadio::factory_implementation
{
	struct FrequencyFormattingConverter : FrequencyFormattingConverterT<FrequencyFormattingConverter, implementation::FrequencyFormattingConverter>
	{
	};
}