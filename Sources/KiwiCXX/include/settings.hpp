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

#ifndef SETTINGS_H
#define SETTINGS_H

#include <string>
#include <vector>

struct KiwiSetting
{
    KiwiSetting(std::string name, void *value, bool isString):
    name(name), value(value), isString(isString) {}
    
    std::string name;
    void *value;
    bool isString;
};

class KiwiSettings
{
public:
    static int upscalingFilter;
    static int upscalingFactor;
    
    static void add(std::vector<KiwiSetting> platformSettings);
    static bool load(std::string filename = "config.ini");
    static bool save();
    
private:
    static std::string filename;
    static std::vector<KiwiSetting> settings;
    
    KiwiSettings() {} // Private to prevent instantiation
};

#endif // SETTINGS_H
