#ifndef JSONSAVEVISITOR_H
#define JSONSAVEVISITOR_H

#include "ConstitemVisitor.h"
#include <QJsonObject>
#include <QJsonArray>
#include "Event.h"
#include "Reminder.h"
#include "ToDo.h"

class JsonSaveVisitor : public ConstItemVisitor {
private:
    QJsonArray jsonArray;

    // Per evitare duplicazione dei campi comuni
    QJsonObject createBaseObject(const Activity& a, const QString& type) {
        QJsonObject obj;
        obj["type"] = type;
        obj["title"] = a.getTitle();
        obj["description"] = a.getDescription();
        return obj;
    }

public:
    JsonSaveVisitor() = default;

    void visit(const Event& event) override {
        QJsonObject obj = createBaseObject(event, "Event");
        obj["startDate"] = event.getStartDate().toString(Qt::ISODate);
        obj["endDate"] = event.getEndDate().toString(Qt::ISODate);
        obj["location"] = event.getLocation();
        jsonArray.append(obj);
    }

    void visit(const Reminder& reminder) override {
        QJsonObject obj = createBaseObject(reminder, "Reminder");
        obj["dateOfNotification"] = reminder.getDateOfNotification().toString(Qt::ISODate);
        obj["frequency"] = static_cast<int>(reminder.getFrequency());
        obj["completed"] = reminder.isCompleted();
        jsonArray.append(obj);
    }

    void visit(const ToDo& todo) override {
        QJsonObject obj = createBaseObject(todo, "ToDo");
        obj["priority"] = static_cast<int>(todo.getPriority());
        obj["completed"] = todo.isCompleted();
        jsonArray.append(obj);
    }

    QJsonArray getResult() const { return jsonArray; }
};

#endif // JSONSAVEVISITOR_H
