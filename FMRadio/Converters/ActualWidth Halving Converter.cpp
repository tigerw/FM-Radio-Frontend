#include "pch.h"
#include "ActualWidth Halving Converter.h"
#include "ActualWidthHalvingConverter.g.cpp"

namespace winrt::FMRadio::implementation
{
	Windows::Foundation::IInspectable ActualWidthHalvingConverter::Convert(Windows::Foundation::IInspectable const & Value, Windows::UI::Xaml::Interop::TypeName const &, Windows::Foundation::IInspectable const &, hstring const &)
	{
		return box_value(unbox_value<double>(Value) / 2);
	}

	Windows::Foundation::IInspectable ActualWidthHalvingConverter::ConvertBack(Windows::Foundation::IInspectable const &, Windows::UI::Xaml::Interop::TypeName const &, Windows::Foundation::IInspectable const &, hstring const &)
	{
		throw hresult_not_implemented();
	}
}