#ifndef MODEL_ITEMS_TODO_H
#define MODEL_ITEMS_TODO_H

#include "Activity.h"

class ToDo: public Activity {
public:
    enum priorityOfTodo {
        None = 0,
        Low,
        Medium,
        High
    };
private:
    priorityOfTodo priority;
    bool completed;
public:
    ToDo(const QString& t, const QString& d = "", priorityOfTodo p = None, bool c = false);

    priorityOfTodo getPriority() const;
    void setPriority(priorityOfTodo p);
    bool getCompleted() const;

    void invertCompleted();
    bool isCompleted() const override;

    void accept(ItemVisitor& visitor) override;
    void accept(ConstItemVisitor& visitor) const override;
};


#endif //MODEL_ITEMS_TODO_H
