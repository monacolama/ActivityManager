#ifndef MODEL_UTILITY_CONSTITEMVISITOR_H
#define MODEL_UTILITY_CONSTITEMVISITOR_H

class ToDo;
class Event;
class Reminder;

class ConstItemVisitor {
public:
    virtual ~ConstItemVisitor() = default;

    virtual void visit(const ToDo& todo) = 0;
    virtual void visit(const Event& event) = 0;
    virtual void visit(const Reminder& reminder) = 0;
};

#endif //MODEL_UTILITY_CONSTITEMVISITOR_H
