#include "uivisitor.h"

UiVisitor::UiVisitor(QTreeWidget* tree, QListWidget* list, int view,
                     std::function<void(Activity*)> editCb, std::function<void(Activity*)> delCb)
    : mainTree(tree), todoList(list), viewMode(view), today(QDate::currentDate()), onEditClick(editCb), onDeleteClick(delCb) {}

void UiVisitor::visit(const Event& e) {

    // Controllo su quale pagina mostrare l'evento tra Today, All e Completed
    bool show = false;
    if (viewMode == 1) show = true;
    else if (viewMode == 2) show = e.isCompleted();
    else if (viewMode == 0) show = e.appearsOnDate(today);

    if (show) {
        QTreeWidgetItem* root = mainTree->topLevelItem(0); // Cartella degli eventi
        if (root) {

            QTreeWidgetItem* rootItem = new QTreeWidgetItem(root);
            rootItem->setText(0, e.getTitle());

            QFont font = rootItem->font(0);
            if (e.isCompleted()) {
                font.setStrikeOut(true);
                rootItem->setFont(0, font);
                rootItem->setForeground(0, QBrush(Qt::gray));
            } else {
                font.setBold(true);
                rootItem->setFont(0, font);
            }

            if (e.inProgress()) {
                rootItem->setData(0, Qt::UserRole, true);
            }

            QPushButton* btnEdit = new QPushButton();
            btnEdit->setIcon(QIcon(":/model/icons/edit-button.png"));
            btnEdit->setIconSize(QSize(18, 18));
            btnEdit->setStyleSheet("background: transparent; border: none;");
            btnEdit->setCursor(Qt::PointingHandCursor);
            btnEdit->setFixedSize(24, 24);

            QPushButton* btnDelete = new QPushButton();
            btnDelete->setIcon(QIcon(":/model/icons/delete-button.png"));
            btnDelete->setIconSize(QSize(18, 18));
            btnDelete->setStyleSheet("background: transparent; border: none;");
            btnDelete->setCursor(Qt::PointingHandCursor);
            btnDelete->setFixedSize(24, 24);

            QWidget* container = new QWidget();
            QHBoxLayout* layout = new QHBoxLayout(container);
            layout->setContentsMargins(0, 0, 50, 0);
            layout->setSpacing(5);
            layout->addStretch();
            layout->addWidget(btnEdit);
            layout->addWidget(btnDelete);

            mainTree->setItemWidget(rootItem, 1, container);

            // Connessioni coi bottoni di modifica e cancellazione
            Activity* actPtr = const_cast<Event*>(&e);

            auto editCbCopy = this->onEditClick;
            auto deleteCbCopy = this->onDeleteClick;

            QObject::connect(btnEdit, &QPushButton::clicked, [editCbCopy, actPtr]() {
                if(editCbCopy) editCbCopy(actPtr);
            });

            QObject::connect(btnDelete, &QPushButton::clicked, [deleteCbCopy, actPtr]() {
                if(deleteCbCopy) deleteCbCopy(actPtr);
            });


            QTreeWidgetItem* itemDate = new QTreeWidgetItem(rootItem);
            QString orari = e.getStartDate().toString("dd/MM") + "-" + e.getEndDate().toString("dd/MM") + " | " +
                            e.getStartDate().toString("hh:mm") + "-" + e.getEndDate().toString("hh:mm");
            itemDate->setText(0, orari);
            itemDate->setIcon(0, QIcon(":/model/icons/clock-button.png"));
            itemDate->setForeground(0, QColor(179, 179, 179));

            if (!e.getDescription().isEmpty()) {
                QTreeWidgetItem* itemDesc = new QTreeWidgetItem(rootItem);
                itemDesc->setText(0, e.getDescription());
                itemDesc->setForeground(0, QColor(179, 179, 179));
                itemDesc->setIcon(0, QIcon(":/model/icons/description-button.png"));
                itemDesc->setToolTip(0, e.getDescription());
            }

            if (!e.getLocation().isEmpty()) {
                QTreeWidgetItem* itemLoc = new QTreeWidgetItem(rootItem);
                itemLoc->setText(0, e.getLocation());
                itemLoc->setIcon(0, QIcon(":/model/icons/location-button.png"));
                itemLoc->setForeground(0, QColor(179, 179, 179));
            }



        }
    }
}

void UiVisitor::visit(const Reminder& r) {

    bool show = false;
    if (viewMode == 1) show = true;
    if (viewMode == 2 && r.isCompleted()) show = r.isCompleted();
    if (viewMode == 0 && r.getDateOfNotification().date() == today) show = true;

    if (show) {
        QTreeWidgetItem* root = mainTree->topLevelItem(1); // Cartella dei promemoria
        if (root) {

            QTreeWidgetItem* rootItem = new QTreeWidgetItem(root);
            rootItem->setText(0, r.getTitle());

            QFont font = rootItem->font(0);
            if (r.isCompleted()) {
                font.setStrikeOut(true);
                rootItem->setFont(0, font);
                rootItem->setForeground(0, QBrush(Qt::gray));
            } else {
                font.setBold(true);
                rootItem->setFont(0, font);
            }

            QPushButton* btnEdit = new QPushButton();
            btnEdit->setIcon(QIcon(":/model/icons/edit-button.png"));
            btnEdit->setIconSize(QSize(18, 18));
            btnEdit->setStyleSheet("background: transparent; border: none;");
            btnEdit->setCursor(Qt::PointingHandCursor);
            btnEdit->setFixedSize(24, 24);

            QPushButton* btnDelete = new QPushButton();
            btnDelete->setIcon(QIcon(":/model/icons/delete-button.png"));
            btnDelete->setIconSize(QSize(18, 18));
            btnDelete->setStyleSheet("background: transparent; border: none;");
            btnDelete->setCursor(Qt::PointingHandCursor);
            btnDelete->setFixedSize(24, 24);

            QWidget* container = new QWidget();
            QHBoxLayout* layout = new QHBoxLayout(container);
            layout->setContentsMargins(0, 0, 50, 0);
            layout->setSpacing(5);
            layout->addStretch();
            layout->addWidget(btnEdit);
            layout->addWidget(btnDelete);

            mainTree->setItemWidget(rootItem, 1, container);

            Activity* actPtr = const_cast<Reminder*>(&r);

            auto editCbCopy = this->onEditClick;
            auto deleteCbCopy = this->onDeleteClick;

            QObject::connect(btnEdit, &QPushButton::clicked, [editCbCopy, actPtr]() {
                if(editCbCopy) editCbCopy(actPtr);
            });

            QObject::connect(btnDelete, &QPushButton::clicked, [deleteCbCopy, actPtr]() {
                if(deleteCbCopy) deleteCbCopy(actPtr);
            });

            QTreeWidgetItem* itemNotif = new QTreeWidgetItem(rootItem);
            itemNotif->setText(0, r.getDateOfNotification().toString("dd/MM/yy | hh:mm"));
            itemNotif->setForeground(0, QColor(179, 179, 179));
            itemNotif->setIcon(0, QIcon(":/model/icons/notification-button.png"));

            if (!r.getDescription().isEmpty()) {
                QTreeWidgetItem* itemDesc = new QTreeWidgetItem(rootItem);
                itemDesc->setText(0,r.getDescription());
                itemDesc->setForeground(0, QColor(179, 179, 179));
                itemDesc->setIcon(0, QIcon(":/model/icons/description-button.png"));
                itemDesc->setToolTip(0, r.getDescription());
            }

            QTreeWidgetItem* itemFreq = new QTreeWidgetItem(rootItem);
            QString freqText;
            if (r.getFrequency() == 1) freqText = "Daily";
            else if (r.getFrequency() == 2) freqText = "Weekly";
            else if (r.getFrequency() == 3) freqText = "Monthly";
            else if (r.getFrequency() == 4) freqText = "Yearly";
            else freqText = "Never";
            itemFreq->setText(0, freqText);
            itemFreq->setForeground(0, QColor(179, 179, 179));
            itemFreq->setIcon(0, QIcon(":/model/icons/repeat-button.png"));
        }
    }
}

void UiVisitor::visit(const ToDo& t) {


    QString displayTitle = t.getTitle();
    if (t.getPriority() > 0) {
        // Aggiunta carattere "!" al titolo in base alla priorità
        displayTitle = QString("!").repeated(t.getPriority()) + " " + displayTitle;
    }

    if (!t.isCompleted()) {
        QListWidgetItem* listItem = new QListWidgetItem(todoList);
        listItem->setFlags(listItem->flags() | Qt::ItemIsUserCheckable);
        listItem->setCheckState(Qt::Unchecked);
        listItem->setData(Qt::UserRole, QVariant::fromValue(static_cast<void*>(const_cast<ToDo*>(&t))));

        listItem->setText(displayTitle);
    }

    bool show = (viewMode == 1) || (viewMode == 2 && t.isCompleted());

    if (show) {
        QTreeWidgetItem* root = mainTree->topLevelItem(2); // Cartella ToDo
        if (root) {

            QTreeWidgetItem* rootItem = new QTreeWidgetItem(root);
            rootItem->setText(0, displayTitle);

            QFont font = rootItem->font(0);
            if (t.isCompleted()) {
                font.setStrikeOut(true);
                rootItem->setForeground(0, QBrush(Qt::gray));
            } else {
                font.setBold(true);
            }
            rootItem->setFont(0, font);

            QPushButton* btnEdit = new QPushButton();
            btnEdit->setIcon(QIcon(":/model/icons/edit-button.png"));
            btnEdit->setIconSize(QSize(18, 18));
            btnEdit->setStyleSheet("background: transparent; border: none;");
            btnEdit->setCursor(Qt::PointingHandCursor);
            btnEdit->setFixedSize(24, 24);

            QPushButton* btnDelete = new QPushButton();
            btnDelete->setIcon(QIcon(":/model/icons/delete-button.png"));
            btnDelete->setIconSize(QSize(18, 18));
            btnDelete->setStyleSheet("background: transparent; border: none;");
            btnDelete->setCursor(Qt::PointingHandCursor);
            btnDelete->setFixedSize(24, 24);

            QWidget* container = new QWidget();
            QHBoxLayout* layout = new QHBoxLayout(container);
            layout->setContentsMargins(0, 0, 50, 0);
            layout->setSpacing(5);
            layout->addStretch();
            layout->addWidget(btnEdit);
            layout->addWidget(btnDelete);

            mainTree->setItemWidget(rootItem, 1, container);

            // Connessioni coi bottoni di modifica e cancellazione
            Activity* actPtr = const_cast<ToDo*>(&t);
            auto editCbCopy = this->onEditClick;
            auto deleteCbCopy = this->onDeleteClick;

            QObject::connect(btnEdit, &QPushButton::clicked, [editCbCopy, actPtr]() {
                if(editCbCopy) editCbCopy(actPtr);
            });

            QObject::connect(btnDelete, &QPushButton::clicked, [deleteCbCopy, actPtr]() {
                if(deleteCbCopy) deleteCbCopy(actPtr);
            });

            if (!t.getDescription().isEmpty()) {
                QTreeWidgetItem* itemDesc = new QTreeWidgetItem(rootItem);
                itemDesc->setText(0, t.getDescription());
                itemDesc->setForeground(0, QBrush(Qt::gray));
                itemDesc->setIcon(0, QIcon(":/model/icons/description-button.png"));
                itemDesc->setToolTip(0, t.getDescription());
            }
        }
    }
}
