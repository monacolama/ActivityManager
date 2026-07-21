#ifndef UIVISITOR_H
#define UIVISITOR_H

#include <QDate>
#include <functional>
#include <QString>
#include <QIcon>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QListWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QFont>
#include <QColor>
#include <QVariant>
#include <QPainter>

#include "ConstitemVisitor.h"
#include "Activity.h"
#include "Event.h"
#include "Reminder.h"
#include "ToDo.h"

class QTreeWidget;
class QListWidget;

class UiVisitor : public ConstItemVisitor {
public:

    static QIcon getWhiteIcon(const QString& ip);

    UiVisitor(QTreeWidget* tree, QListWidget* list, int viewMode,
              std::function<void(Activity*)> onEditClick, std::function<void(Activity*)> onDeleteClick);

    void visit(const Event& e) override;
    void visit(const Reminder& r) override;
    void visit(const ToDo& t) override;

private:

    QTreeWidget* mainTree;
    QListWidget* todoList;
    int viewMode;
    QDate today;
    std::function<void(Activity*)> onEditClick;
    std::function<void(Activity*)> onDeleteClick;
};

#endif // UIVISITOR_H
