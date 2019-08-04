#include "pch.h"
#include "Frequency Formatting Converter.h"
#include "FrequencyFormattingConverter.g.cpp"

namespace winrt::FMRadio::implementation
{
	hstring FrequencyFormattingConverter::Convert(hstring TextualFrequency)
	{
		const unsigned Kilo = 3;
		const auto Frequency = std::wstring_view(TextualFrequency);

		if (Frequency.length() < Kilo)
		{
			return TextualFrequency;
		}

		const auto IntegerPartLength = Frequency.length() - Kilo;
		const auto IntegerPart = Frequency.substr(0, IntegerPartLength);
		const auto FractionalPart = Frequency.substr(IntegerPartLength, 1);
		return hstring(IntegerPart) + std::use_facet<std::numpunct<wchar_t>>(std::locale()).decimal_point() + FractionalPart;
	}

	Windows::Foundation::IInspectable FrequencyFormattingConverter::Convert(Windows::Foundation::IInspectable const & Value, Windows::UI::Xaml::Interop::TypeName const &, Windows::Foundation::IInspectable const &, hstring const &)
	{
		return box_value(Convert(unbox_value<hstring>(Value)));
	}

	Windows::Foundation::IInspectable FrequencyFormattingConverter::ConvertBack(Windows::Foundation::IInspectable const &, Windows::UI::Xaml::Interop::TypeName const &, Windows::Foundation::IInspectable const &, hstring const &)
	{
		throw hresult_not_implemented();
	}
}