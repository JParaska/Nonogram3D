// Created by Jan Paraska. All rights reserved


#include "N3DNonogramColorScheme.h"


TArray<float> FN3DColorScheme::Get() const
{
    TArray<float> Data;

    Data.Add(Color.R);
    Data.Add(Color.G);
    Data.Add(Color.B);

    Data.Add(ColorIntensity);

    Data.Add(EdgeSharpness);

    Data.Add(EdgeSubtraction);

    return Data;
}
