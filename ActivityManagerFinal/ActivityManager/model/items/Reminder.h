#ifndef MODEL_ITEMS_REMINDER_H
#define MODEL_ITEMS_REMINDER_H

#include "Activity.h"
#include <QDateTime>
#include <exception>

class Reminder: public Activity {
public:
    enum frequencyOfRepetition {
        Never = 0,
        Daily,
        Weekly,
        Monthly,
        Yearly
    };
private:
    QDateTime dateOfNotification;
    frequencyOfRepetition frequency;
    bool isRang;
public:
    Reminder(const QString& t, const QString& d, const QDateTime& dt, frequencyOfRepetition f = Never);
    QDateTime getDateOfNotification() const;
    void setDateOfNotification(const QDateTime& dt);
    frequencyOfRepetition getFrequency() const;
    void setFrequency(const frequencyOfRepetition& f);

    void snooze(int minutes);
    bool hasToRing() const;
    bool confirmRead();

    bool isCompleted() const override;

    void accept(ItemVisitor& visitor) override;
    void accept(ConstItemVisitor& visitor) const override;
};


#endif //MODEL_ITEMS_REMINDER_H
