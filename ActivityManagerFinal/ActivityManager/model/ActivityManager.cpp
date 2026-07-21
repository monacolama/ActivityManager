#include "ActivityManager.h"


ActivityManager::~ActivityManager() {
    for (Activity* a : activities) {
        delete a;
    }
    activities.clear();
}

void ActivityManager::addActivity(Activity* a) {
    if (a != nullptr) {
        activities.push_back(a);
    }
}

void ActivityManager::removeActivity(Activity* a) {
    for (auto it = activities.begin(); it != activities.end(); ++it) {
        if(*it == a) {
            delete a;
            activities.erase(it);

            break;
        }
    }
}

const std::vector<Activity*>& ActivityManager::getAllActivities() const {
    return activities;
}

std::vector<Activity*> ActivityManager::searchActivity(const SearchFilter& filter) const {
    std::vector<Activity*> results;

    for (Activity* a : activities) {

        if (!filter.keyword.isEmpty()) {
            bool titleMatches = a->getTitle().contains(filter.keyword, Qt::CaseInsensitive);
            bool descMatches = a->getDescription().contains(filter.keyword, Qt::CaseInsensitive);
            if (!titleMatches && !descMatches) {
                continue;
            }
        }

        // Filtro stato dell'attività
        if (filter.status == SearchFilter::OnlyCompleted && !a->isCompleted()) continue;
        if (filter.status == SearchFilter::OnlyPending && a->isCompleted()) continue;

        // Filtro tipo
        if (filter.type == SearchFilter::OnlyEvents && !dynamic_cast<Event*>(a)) continue;
        if (filter.type == SearchFilter::OnlyReminders && !dynamic_cast<Reminder*>(a)) continue;
        if (filter.type == SearchFilter::OnlyToDos && !dynamic_cast<ToDo*>(a)) continue;

        // Filtro data
        if (filter.useDateFilter) {
            bool isWithinDate = false;

            if (Event* e = dynamic_cast<Event*>(a)) {

                QDate eventStart = e->getStartDate().date();
                QDate eventEnd = e->getEndDate().date();
                if (eventStart <= filter.endDate && eventEnd >= filter.startDate) {
                    isWithinDate = true;
                }
            }
            else if (Reminder* r = dynamic_cast<Reminder*>(a)) {

                QDate notifDate = r->getDateOfNotification().date();
                if (notifDate >= filter.startDate && notifDate <= filter.endDate) {
                    isWithinDate = true;
                }
            }

            if (!isWithinDate) {
                continue;
            }
        }
        results.push_back(a);
    }

    return results;
}

void ActivityManager::saveToFile(const QString& filePath) const {
    JsonSaveVisitor visitor;

    for (Activity* act : activities) {
        if (act) {
            act->accept(visitor);
        }
    }

    QJsonDocument doc(visitor.getResult());

    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(doc.toJson());
        file.close();
    }
}

void ActivityManager::loadFromFile(const QString& filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) return;

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();

    if (!doc.isArray()) return;

    // Svuota i vecchi dati prima di caricare i nuovi
    for (Activity* act : activities) {
        delete act;
    }
    activities.clear();

    QJsonArray array = doc.array();
    for (QJsonValue val : array) {
        QJsonObject obj = val.toObject();

        QString type = obj["type"].toString();
        QString title = obj["title"].toString();
        QString desc = obj["description"].toString();

        try {
            if (type == "Event") {
                QDateTime start = QDateTime::fromString(obj["startDate"].toString(), Qt::ISODate);
                QDateTime end = QDateTime::fromString(obj["endDate"].toString(), Qt::ISODate);
                QString loc = obj["location"].toString();

                activities.push_back(new Event(title, desc, start, end, loc));
            }
            else if (type == "Reminder") {
                QDateTime dateNot = QDateTime::fromString(obj["dateOfNotification"].toString(), Qt::ISODate);
                Reminder::frequencyOfRepetition freq = static_cast<Reminder::frequencyOfRepetition>(obj["frequency"].toInt());

                Reminder* r = new Reminder(title, desc, dateNot, freq);
                if (obj["completed"].toBool() && !r->isCompleted()) r->confirmRead();

                activities.push_back(r);
            }
            else if (type == "ToDo") {
                ToDo::priorityOfTodo prio = static_cast<ToDo::priorityOfTodo>(obj["priority"].toInt());

                ToDo* t = new ToDo(title, desc, prio);
                if (obj["completed"].toBool() && !t->isCompleted()) t->invertCompleted();

                activities.push_back(t);
            }
        }
        catch (const std::exception& e) {
            qWarning() << "Skipped invalid activity during loading: " << e.what();
        }
    }
}
