#include "Activity.h"

Activity::Activity(const QString& t, const QString d): title(t), description(d) {}

QString Activity::getTitle() const {
    return title;
}

void Activity::setTitle(const QString& t) {
    title = t;
}

QString Activity::getDescription() const {
    return description;
}

void Activity::setDescription(const QString& d) {
    description = d;
}
