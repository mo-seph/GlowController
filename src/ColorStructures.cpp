#include "ColorStructures.h"


void FHSV::toRGBW(FRGBW& target) {
    HSVtoRGBW(*this,target);
}