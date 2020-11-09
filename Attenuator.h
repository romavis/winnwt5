#ifndef ATTENUATOR_H
#define ATTENUATOR_H

#include <QMap>
#include <QVector>
#include <QString>
#include <QStringList>

///
/// \brief The Attenuator class.
/// Provides a list of attenuation values supported by the attenuator
/// and converts attenuation to a specific control code for the attenuator.
///
class Attenuator
{
friend class AttenuatorLoader;

private:
    QMap<int, int> _attenToControlCodeMap;
    QMap<int, int> _controlCodeToAttenMap;
    QMap<int, int> _attenToIndexMap;
    QVector<int> _attenuations;

protected:
    void AddControlCode(int attenuation_db, int code);
    void Clear();

public:
    Attenuator();

    const QVector<int> GetAttenuations();
    int GetControlCode(int attenuation_db);
    int GetControlCodeByIndex(int index);
    int GetIndex(int attenuation_db);
    int GetAttenuationByCode(int controlCode);
    bool CanAttenuateBy(int attenuation_db);

    int GetMaxAttenuation();
};

enum AttenuatorType
{
    AttenuatorType_Standard,
    AttenuatorType_Fa,
    AttenuatorType_Custom,
};

class AttenuatorLoader
{
public:
    void static Load(Attenuator &atten, AttenuatorType type, const QString &config);

    void static LoadStandard(Attenuator &atten);
    void static LoadFa(Attenuator &atten);
    void static LoadCustom(Attenuator &atten, const QString &config);

    bool static IsCustomConfigValid(const QString &config);
    QString static GetEmptyCustomConfig();

private:
    static const int _maxCustomStages;
    static const int _maxStageAtten;
    static const char _customStageConfigSeparator;

    bool static TryParseConfig(const QString &config, QVector<int> &stages);
    int static GetControlCodeByFaIndex(int index, int *attenuation_db);
};

#endif // ATTENUATOR_H
