#ifndef MODEL_ACTIVITYMANAGER_H
#define MODEL_ACTIVITYMANAGER_H

#include <vector>
#include <QString>
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QDateTime>
#include <QDate>
#include "Event.h"
#include "Reminder.h"
#include "ToDo.h"
#include "Activity.h"
#include "JsonSaveVisitor.h"

// Struttura per filtri di ricerca
struct SearchFilter {
    QString keyword = "";

    enum TypeFilter { AllTypes, OnlyEvents, OnlyReminders, OnlyToDos };
    TypeFilter type = AllTypes;

    enum StatusFilter { AnyStatus, OnlyCompleted, OnlyPending };
    StatusFilter status = AnyStatus;

    bool useDateFilter = false;
    QDate startDate;
    QDate endDate;
};

class ActivityManager {
private:
    std::vector<Activity*> activities;
public:
    ActivityManager() = default;
    ~ActivityManager();

    void addActivity(Activity* a);
    void removeActivity(Activity* a);
    const std::vector<Activity*>& getAllActivities() const;
    std::vector<Activity*> searchActivity(const SearchFilter& filter) const;

    void saveToFile(const QString& filePath) const;
    void loadFromFile(const QString& filePath);
};


#endif //MODEL_ACTIVITYMANAGER_H
