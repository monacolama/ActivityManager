#include "Event.h"
#include "../Utility/ItemVisitor.h"
#include "../Utility/ConstitemVisitor.h"

Event::Event(const QString& t, const QString& d, const QDateTime& sdt, const QDateTime& edt, const QString& l): Activity(t, d), location(l) {

    if (edt < sdt) {
        throw std::invalid_argument("Error: the end date precedes the start date");
    }

    startDate = sdt;
    endDate = edt;
}

QDateTime Event::getStartDate() const {
    return startDate;
}

void Event::setStartDate(const QDateTime& sdt) {
    if (endDate < sdt) {
        throw std::invalid_argument("Error: the new start date would exceed the current event end date");
    }

    startDate = sdt;
}

QDateTime Event::getEndDate() const {
    return endDate;
}

void Event::setEndDate(const QDateTime& edt) {
    if (edt < startDate) {
        throw std::invalid_argument("Error: the new end date precedes the current event start date");
    }

    endDate = edt;
}

QString Event::getLocation() const {
    return location;
}

void Event::setLocation(const QString& l) {
    location = l;
}

bool Event::inProgress() const {
    QDateTime now = QDateTime::currentDateTime();
    return (now >= startDate) && (now <= endDate);
}

bool Event::appearsOnDate(const QDate& dt) const {
    QDateTime startOfDay(dt, QTime(0, 0, 0));
    QDateTime endOfDay(dt, QTime(23, 59, 59));

    return (startDate <= endOfDay) && (endDate >= startOfDay);
}

bool Event::isCompleted() const {
    return QDateTime::currentDateTime() > endDate;
}

void Event::accept(ItemVisitor& visitor) {
    visitor.visit(*this);
}

void Event::accept(ConstItemVisitor& visitor) const {
    visitor.visit(*this);
}
