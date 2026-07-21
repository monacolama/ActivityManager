#ifndef MODEL_UTILITY_ITEMVISITOR_H
#define MODEL_UTILITY_ITEMVISITOR_H

class ToDo;
class Event;
class Reminder;

class ItemVisitor {
public:
    virtual ~ItemVisitor() = default;

    virtual void visit(ToDo& todo) = 0;
    virtual void visit(Event& event) = 0;
    virtual void visit(Reminder& reminder) = 0;
};

#endif //MODEL_UTILITY_ITEMVISITOR_H
