#pragma once

#include "ActualWidthHalvingConverter.g.h"

namespace winrt::FMRadio::implementation
{
	struct ActualWidthHalvingConverter : ActualWidthHalvingConverterT<ActualWidthHalvingConverter>
	{
		Windows::Foundation::IInspectable Convert(Windows::Foundation::IInspectable const &, Windows::UI::Xaml::Interop::TypeName const &, Windows::Foundation::IInspectable const &, hstring const &);
		Windows::Foundation::IInspectable ConvertBack(Windows::Foundation::IInspectable const &, Windows::UI::Xaml::Interop::TypeName const &, Windows::Foundation::IInspectable const &, hstring const &);
	};
}

namespace winrt::FMRadio::factory_implementation
{
	struct ActualWidthHalvingConverter : ActualWidthHalvingConverterT<ActualWidthHalvingConverter, implementation::ActualWidthHalvingConverter>
	{
	};
}