#include "ToDo.h"
#include "../Utility/ItemVisitor.h"
#include "../Utility/ConstitemVisitor.h"

ToDo::ToDo(const QString& t, const QString& d, priorityOfTodo p, bool c): Activity(t, d), priority(p), completed(c) {}

ToDo::priorityOfTodo ToDo::getPriority() const {
    return priority;
}

void ToDo::setPriority(priorityOfTodo p) {
    priority = p;
}

bool ToDo::getCompleted() const {
    return completed;
}

void ToDo::invertCompleted() {
    completed = !completed;
}

bool ToDo::isCompleted() const {
    return completed;
}

void ToDo::accept(ItemVisitor& visitor) {
    visitor.visit(*this);
}

void ToDo::accept(ConstItemVisitor& visitor) const {
    visitor.visit(*this);
}
