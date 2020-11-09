/***************************************************************************

    attenuator.cpp  -  description
    ----------------------------------------------------
    begin                : 2016
    copyright            : (C) 2016 by serge_m
    email                :

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "Attenuator.h"
#include <QtAlgorithms>


// Attenuator
void Attenuator::AddControlCode(int attenuation_db, int code)
{
    _attenuations.append(attenuation_db);
    _attenToControlCodeMap[attenuation_db] = code;
    _controlCodeToAttenMap[code] = attenuation_db;
    _attenToIndexMap[attenuation_db] = _attenuations.count() - 1;
}

void Attenuator::Clear()
{
    _attenToControlCodeMap.clear();
    _controlCodeToAttenMap.clear();
    _attenToIndexMap.clear();
    _attenuations.clear();
}

Attenuator::Attenuator()
{
    AddControlCode(0, 0);
}

const QVector<int> Attenuator::GetAttenuations()
{
    return _attenuations;
}

int Attenuator::GetControlCode(int attenuation_db)
{
    if (_attenToControlCodeMap.contains(attenuation_db))
    {
        return _attenToControlCodeMap[attenuation_db];
    }
    return 0;
}

int Attenuator::GetControlCodeByIndex(int index)
{
    if (index >= 0 && index < _attenuations.count())
    {
        return GetControlCode(_attenuations[index]);
    }
    return 0;
}

int Attenuator::GetIndex(int attenuation_db)
{
    if (_attenToIndexMap.contains(attenuation_db))
    {
        return _attenToIndexMap[attenuation_db];
    }
    return 0;
}

int Attenuator::GetAttenuationByCode(int controlCode)
{
    if (_controlCodeToAttenMap.contains(controlCode))
    {
        return _controlCodeToAttenMap[controlCode];
    }
    return 0;
}

bool Attenuator::CanAttenuateBy(int attenuation_db)
{
    return _attenToControlCodeMap.contains(attenuation_db);
}

int Attenuator::GetMaxAttenuation()
{
    return _attenuations.count() > 0 ? _attenuations.last() : 0;
}

// AttenuatorLoader
void AttenuatorLoader::Load(Attenuator &atten, AttenuatorType type, const QString &config)
{
    switch (type)
    {
    case AttenuatorType_Fa:
        LoadFa(atten);
        break;

    case AttenuatorType_Custom:
        LoadCustom(atten, config);
        break;

    case AttenuatorType_Standard:
    default:
        // TODO: do something if arguments are invalid
        LoadStandard(atten);
        break;
    }
}

void AttenuatorLoader::LoadStandard(Attenuator &atten)
{
    atten.Clear();
    atten.AddControlCode(0,  0x00);
    atten.AddControlCode(10, 0x01);
    atten.AddControlCode(20, 0x02);
    atten.AddControlCode(30, 0x03);
    atten.AddControlCode(40, 0x06);
    atten.AddControlCode(50, 0x07);
}

void AttenuatorLoader::LoadFa(Attenuator &atten)
{
    atten.Clear();
    atten.AddControlCode(0, 0);

    for (int i = 1; i < 33; ++i)
    {
        int attenuation = 0;
        int controlCode = GetControlCodeByFaIndex(i, &attenuation);
        atten.AddControlCode(attenuation, controlCode);
    }
}

int AttenuatorLoader::GetControlCodeByFaIndex(int index, int *attenuation_db)
{
    bool r1 = false, // -2dB
         r2 = false, // -4dB
         r3 = false, // -8db
         r4 = false, // -4dB
         r5 = false, // -16dB
         r6 = false; // -32db

    int db = (index+1) * 2;
    *attenuation_db = db;

    if(db == 32 or db >= 36)
    {
        r6 = true;
        db = db - 32;
    }
    if(db == 16 or db >= 20)
    {
        r5 = true;
        db = db - 16;
    }
    if(db != 0)
    {
        db = db - 4;
        r4 = true;
        if(db >= 8)
        {
            r3 = true;
            db = db - 8;
        }
        if(db >= 4)
        {
            r2 = true;
            db = db - 4;
        }
        if(db >= 2)
        {
            r1 = true;
            db = db - 2;
        }
    }

    int portb = 0;
    if(r1)
    {
        portb += 1;
    }
    if(r2)
    {
        portb += 2;
    }
    if(r3)
    {
        portb += 4;
    }
    if(r4)
    {
        portb += 8;
    }
    if(r5)
    {
        portb += 16;
    }
    if(r6)
    {
        portb += 32;
    }

    return portb;
}

class CustomAttenCode
{
private:
    int _code;
    int _atten;
    long _priority;

public:
    CustomAttenCode()
        : _code(0), _atten(0), _priority(0)
    {
    }

    CustomAttenCode(const QVector<int> &stages, int code)
        : _code(code), _atten(0), _priority(0)
    {
        // Go through the stages to calculate total attenuation
        for (int stageNum = 0; stageNum < stages.count(); ++stageNum)
        {
            if ((code & (1 << stageNum)) != 0) // if stage is enabled by the attenuator control code
            {
                int stageAtten = stages[stageNum];
                if (stageAtten <= 0)
                {
                    // Stage is disabled, so the code is invalid
                    _atten = 0;
                    _priority = 0;
                    return;
                }
                _atten += stageAtten;

                // Assume that stages with low attenuation have better linearity,
                // so control codes which use low attenuation stages should have higher priority.
                // Keep lower 8 bits for next prioritization parameter.
                _priority -= ((stageAtten*stageAtten) << 8);
            }
        }

        // Assume that stages with larger stageNum are physically located closer to the output,
        // so they should have higher priority.
        _priority += code;
    }

    int GetCode() const
    {
        return _code;
    }

    int GetAtten() const
    {
        return _atten;
    }

    long GetPriority() const
    {
        return _priority;
    }

    bool IsValid() const
    {
        return _atten > 0;
    }
};

bool LessThanByAttenThenDescByPriority(const CustomAttenCode &c1, const CustomAttenCode &c2)
{
    if (c1.GetAtten() != c2.GetAtten())
    {
        return c1.GetAtten() < c2.GetAtten();
    }

    return c1.GetPriority() > c2.GetPriority();
}

void AttenuatorLoader::LoadCustom(Attenuator &atten, const QString &config)
{
    atten.Clear();
    atten.AddControlCode(0, 0);

    QVector<int> stages;
    if (!TryParseConfig(config, stages))
    {
        // TODO: do something if arguments are invalid
        return;
    }

    // Go through possible codes and selec those which are valid for the current configuration string
    QVector<CustomAttenCode> codes;
    int maxCode = (1 << stages.count()) - 1;

    for (int code = 1; code <= maxCode; ++code)
    {
        CustomAttenCode codeInfo(stages, code);
        if (codeInfo.IsValid())
        {
            codes.push_back(codeInfo);
        }
    }

    // Order codes by attenuation and then by priority
    qSort(codes.begin(), codes.end(), LessThanByAttenThenDescByPriority);

    int previousAtten = 0;
    for (int i = 0; i < codes.count(); ++i)
    {
        CustomAttenCode code = codes[i];
        // Filter out redundant codes
        if (code.GetAtten() != previousAtten)
        {
            atten.AddControlCode(code.GetAtten(), code.GetCode());
            previousAtten = code.GetAtten();
        }
    }
}

bool AttenuatorLoader::IsCustomConfigValid(const QString &config)
{
    QVector<int> dummy;
    return TryParseConfig(config, dummy);
}

const int AttenuatorLoader::_maxCustomStages = 6;
const int AttenuatorLoader::_maxStageAtten = 500; // 500 db per stage should be more than enough for any hardware
const char AttenuatorLoader::_customStageConfigSeparator = ';';

bool AttenuatorLoader::TryParseConfig(const QString &config, QVector<int> &stages)
{
    stages.clear();

    QStringList splitted = config.split(QChar(_customStageConfigSeparator));
    if (splitted.count() != _maxCustomStages)
    {
        return false;
    }

    QVector<int> tmpStages;
    for (QStringList::const_iterator it = splitted.begin(); it != splitted.end(); ++it)
    {
        QString s =it->trimmed();

        if (s.isEmpty())
        {
            // Attenuator stage is disabled
            tmpStages.push_back(0);
        }
        else
        {
            bool isNumber = false;
            int stageAtten = s.toInt(&isNumber);
            if (!isNumber || stageAtten > _maxStageAtten)
            {
                return false;
            }

            tmpStages.push_back(stageAtten);
        }
    }

    stages = tmpStages;
    return true;
}

QString AttenuatorLoader::GetEmptyCustomConfig()
{
    return QString(_maxCustomStages-1, _customStageConfigSeparator);
}
