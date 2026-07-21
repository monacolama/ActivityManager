#ifndef MODEL_ITEMS_EVENT_H
#define MODEL_ITEMS_EVENT_H

#include "Activity.h"
#include <QDateTime>
#include <QDate>
#include <QString>
#include <stdexcept>

class Event: public Activity {
private:
    QDateTime startDate;
    QDateTime endDate;
    QString location;
public:
    Event(const QString& t, const QString& d, const QDateTime& sdt, const QDateTime& edt, const QString& l = "");

    QDateTime getStartDate() const;
    void setStartDate(const QDateTime& sdt);
    QDateTime getEndDate() const;
    void setEndDate(const QDateTime& edt);
    QString getLocation() const;
    void setLocation(const QString& l);

    bool inProgress() const;
    bool appearsOnDate(const QDate& dt) const;

    bool isCompleted() const override;

    void accept(ItemVisitor& visitor) override;
    void accept(ConstItemVisitor& visitor) const override;
};


#endif //MODEL_ITEMS_EVENT_H
