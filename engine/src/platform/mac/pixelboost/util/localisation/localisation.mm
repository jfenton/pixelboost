#include <Foundation/Foundation.h>

#include "pixelboost/util/localisation/localisation.h"

std::string pb::GetCurrentLocale()
{
    std::string locale = [[[NSLocale preferredLanguages] objectAtIndex:0] UTF8String];
    
    int dash = locale.find('-');
    if (dash >= 0)
        return locale.substr(0, dash);
    
    return locale;
}
