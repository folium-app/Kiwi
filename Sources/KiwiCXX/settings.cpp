/*
    Copyright 2019-2023 Hydr8gon

    This file is part of NooDS.

    NooDS is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    NooDS is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
    General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with NooDS. If not, see <https://www.gnu.org/licenses/>.
*/

#include "settings.hpp"
#include "defines.hpp"

int KiwiSettings::upscalingFilter = -1; // -1 = Off, 0 = HQx, 1 = xBRZ
int KiwiSettings::upscalingFactor = 2;

std::string KiwiSettings::filename = "config.ini";

std::vector<KiwiSetting> KiwiSettings::settings =
{
    KiwiSetting("upscalingFilter", &upscalingFilter, false),
    KiwiSetting("upscalingFactor", &upscalingFactor, false)
};

void KiwiSettings::add(std::vector<KiwiSetting> platformSettings)
{
    // Add additional platform settings to be loaded from the settings file
    settings.insert(settings.end(), platformSettings.begin(), platformSettings.end());
}

bool KiwiSettings::load(std::string filename)
{
    // Attempt to open the settings file
    // If the file can't be opened, the default values will be used
    KiwiSettings::filename = filename;
    FILE *settingsFile = fopen(filename.c_str(), "r");
    if (!settingsFile) return false;

    char data[1024];

    // Read each line of the settings file and load the values from them
    while (fgets(data, 1024, settingsFile) != NULL)
    {
        std::string line = data;
        int split = line.find("=");
        std::string name = line.substr(0, split);

        for (unsigned int i = 0; i < settings.size(); i++)
        {
            if (name == settings[i].name)
            {
                std::string value = line.substr(split + 1, line.size() - split - 2);
                if (settings[i].isString)
                    *(std::string*)settings[i].value = value;
                else if (value[0] >= 0x30 && value[0] <= 0x39)
                    *(int*)settings[i].value = stoi(value);
                break;
            }
        }
    }

    fclose(settingsFile);
    return true;
}

bool KiwiSettings::save()
{
    // Attempt to open the settings file
    FILE *settingsFile = fopen(filename.c_str(), "w");
    if (!settingsFile) return false;

    // Write each setting to the settings file
    for (unsigned int i = 0; i < settings.size(); i++)
    {
        std::string value = settings[i].isString ? *(std::string*)settings[i].value : std::to_string(*(int*)settings[i].value);
        fprintf(settingsFile, "%s=%s\n", settings[i].name.c_str(), value.c_str());
    }

    fclose(settingsFile);
    return true;
}
