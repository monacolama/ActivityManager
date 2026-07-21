#ifndef MODEL_ITEMS_ACTIVITY_H
#define MODEL_ITEMS_ACTIVITY_H
#include <QString>

class ItemVisitor;
class ConstItemVisitor;

class Activity {
private:
    QString title;
    QString description;
public:
    Activity(const QString& t, const QString d = "");
    virtual ~Activity() = default;

    QString getTitle() const;
    void setTitle(const QString& t);
    QString getDescription() const;
    void setDescription(const QString& d);

    virtual bool isCompleted() const = 0;

    virtual void accept(ItemVisitor& visitor) = 0;
    virtual void accept(ConstItemVisitor& visitor) const = 0;
};


#endif //MODEL_ITEMS_ACTIVITY_H
