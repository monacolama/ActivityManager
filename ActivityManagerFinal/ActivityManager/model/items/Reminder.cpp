#include "Reminder.h"
#include "../Utility/ItemVisitor.h"
#include "../Utility/ConstitemVisitor.h"

Reminder::Reminder(const QString& t, const QString& d, const QDateTime& dt, frequencyOfRepetition f): Activity(t, d),
    frequency(f), isRang(false) {

    dateOfNotification = dt;
}

QDateTime Reminder::getDateOfNotification() const {
    return dateOfNotification;
}

void Reminder::setDateOfNotification(const QDateTime& dt) {

    dateOfNotification = dt;
}

Reminder::frequencyOfRepetition Reminder::getFrequency() const {
    return frequency;
}

void Reminder::setFrequency(const frequencyOfRepetition& f) {
    frequency = f;
}

void Reminder::snooze(int minutes) {
    dateOfNotification = QDateTime::currentDateTime().addSecs(minutes * 60);
    isRang = false;
}

bool Reminder::hasToRing() const {
    return !isRang && (QDateTime::currentDateTime() >= dateOfNotification);
}

bool Reminder::confirmRead() {
    if (frequency == Never) {
        isRang = true;
        return isRang;
    }

    QDateTime now = QDateTime::currentDateTime();
    while (dateOfNotification <= now) {
        switch (frequency) {
            case Daily: dateOfNotification = dateOfNotification.addDays(1); break;
            case Weekly: dateOfNotification = dateOfNotification.addDays(7); break;
            case Monthly: dateOfNotification = dateOfNotification.addMonths(1); break;
            case Yearly: dateOfNotification = dateOfNotification.addYears(1); break;
            default: break;
        }
    }
    isRang = false;
    return isRang;
}

bool Reminder::isCompleted() const {
    return isRang;
}

void Reminder::accept(ItemVisitor& visitor) {
    visitor.visit(*this);
}

void Reminder::accept(ConstItemVisitor& visitor) const {
    visitor.visit(*this);
}
