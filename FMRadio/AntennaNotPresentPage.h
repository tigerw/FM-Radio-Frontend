#pragma once

#include "AntennaNotPresentPage.g.h"

namespace winrt::FMRadio::implementation
{
    struct AntennaNotPresentPage : AntennaNotPresentPageT<AntennaNotPresentPage>
    {
        AntennaNotPresentPage();
    };
}

namespace winrt::FMRadio::factory_implementation
{
    struct AntennaNotPresentPage : AntennaNotPresentPageT<AntennaNotPresentPage, implementation::AntennaNotPresentPage>
    {
    };
}