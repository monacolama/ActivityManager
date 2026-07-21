#include "mainwindow.h"

// Visitor per apertura form
class FormEditVisitor : public ItemVisitor {
private:
    AddEventDialog* formEvent;
    AddReminderDialog* formReminder;
    AddTodoDialog* formTodo;
    QStackedWidget* formContainer;

public:
    FormEditVisitor(AddEventDialog* e, AddReminderDialog* r, AddTodoDialog* t, QStackedWidget* c)
        : formEvent(e), formReminder(r), formTodo(t), formContainer(c) {}

    void visit(Event& event) override {
        formEvent->loadEvent(&event);
        formContainer->setCurrentIndex(0);
    }

    void visit(Reminder& reminder) override {
        formReminder->loadReminder(&reminder);
        formContainer->setCurrentIndex(1);
    }

    void visit(ToDo& todo) override {
        formTodo->loadTodo(&todo);
        formContainer->setCurrentIndex(2);
    }
};

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent) {

    manager = new ActivityManager();

    currentView = 0; // Pagina Today come pagina di partenza

    // Timer ora attuale nella GUI
    QTimer* clockTimer = new QTimer(this);

    connect(clockTimer, &QTimer::timeout, this, [this]() {
        QString dateAndTime = QDateTime::currentDateTime().toString("dd/MM/yyyy - hh:mm");
        setWindowTitle("Activity Manager | " + dateAndTime);
    });

    clockTimer->start(1000);
    QString startingTime = QDateTime::currentDateTime().toString("dd/MM/yyyy - hh:mm:ss");

    setWindowTitle("Activity Manager | " + startingTime);
    resize(1000, 625);
    setupLayout();

    QTimer* reminderTimer = new QTimer(this);
    connect(reminderTimer, &QTimer::timeout, this, &MainWindow::checkReminders);
    reminderTimer->start(1000);

    QShortcut* shortcutClose = new QShortcut(QKeySequence("Ctrl+W"), this);
    shortcutClose->setContext(Qt::WindowShortcut);
    connect(shortcutClose, &QShortcut::activated, this, &MainWindow::close);
    QShortcut* shortcutSearch = new QShortcut(QKeySequence::Find, this);
    shortcutSearch->setContext(Qt::WindowShortcut);

    connect(shortcutSearch, &QShortcut::activated, this, [this]() {
        searchBar->setFocus();
        searchBar->selectAll();
    });

    // Timer lampeggio eventi
    QTimer* blinkTimer = new QTimer(this);
    connect(blinkTimer, &QTimer::timeout, this, [this]() {
        static bool isRed = false;
        isRed = !isRed;

        QTreeWidgetItemIterator it(mainTree);
        while (*it) {
            if ((*it)->data(0, Qt::UserRole).toBool() == true) {
                if (isRed) {
                    (*it)->setForeground(0, QColor(Qt::red));
                } else {
                    (*it)->setData(0, Qt::ForegroundRole, QVariant());
                }
            }
            ++it;
        }
    });

    blinkTimer->start(500);

    scheduleNextMinuteRefresh();

    // Cambio pagina mostrata in base al bottone selezionato
    connect(btnToday, &QPushButton::clicked, this, [this]() { currentView = 0; refreshUI(); });
    connect(btnAll, &QPushButton::clicked, this, [this]() { currentView = 1; refreshUI(); });
    connect(btnCompleted, &QPushButton::clicked, this, [this]() { currentView = 2; refreshUI(); });

    isShowingPopup = false;

    refreshUI();
}

MainWindow::~MainWindow() { delete manager; }


void MainWindow::refreshUI() {

    if (isShowingPopup) return;

    if (!mainTree || !todoQuickList || !manager) {
        return;
    }

    // Disattivo i segnali per evitare loop
    todoQuickList->blockSignals(true);

    if (currentView == 0) sectionHeader->setText("Today");
    else if (currentView == 1) sectionHeader->setText("All activities");
    else if (currentView == 2) sectionHeader->setText("Completed");

    // Mantengo le liste aperte dall'utente dopo refresh
    QSet<int> expandedCategories;
    QSet<QString> expandedActivities;

    for (int i = 0; i < mainTree->topLevelItemCount(); ++i) {
        QTreeWidgetItem* categoryItem = mainTree->topLevelItem(i);

        if (categoryItem->isExpanded()) {
            expandedCategories.insert(i);
        }
        for (int j = 0; j < categoryItem->childCount(); ++j) {
            QTreeWidgetItem* activityItem = categoryItem->child(j);
            if (activityItem->isExpanded()) {
                expandedActivities.insert(activityItem->text(0));
            }
        }
    }

    int scrollPosition = mainTree->verticalScrollBar()->value();

    // Pulizia completa dell'albero grafico prima del ripopolamento
    todoQuickList->clear();
    for (int i = 0; i < mainTree->topLevelItemCount(); ++i) {
        QTreeWidgetItem* parentItem = mainTree->topLevelItem(i);
        while (parentItem->childCount() > 0) {
            delete parentItem->child(0);
        }
    }

    auto editCallback = [this](Activity* a) {
        this->openEditPanel(a);
    };

    auto deleteCallback = [this](Activity* a) {
        isShowingPopup = true;
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Delete Activity",
                                      "Are you sure you want to delete '" + a->getTitle() + "'?",
                                      QMessageBox::Yes | QMessageBox::No);

        isShowingPopup = false;
        if (reply == QMessageBox::Yes) {
            formEvent->clearForm();
            formReminder->clearForm();
            formTodo->clearForm();
            manager->removeActivity(a);
            formContainer->setVisible(false);
            formSeparator->setVisible(false);
            refreshUI();
        }
    };

    SearchFilter filter;
    filter.keyword = searchBar->text().trimmed();

    if (filterTypeCombo) {
        filter.type = static_cast<SearchFilter::TypeFilter>(filterTypeCombo->currentIndex());
        filter.status = static_cast<SearchFilter::StatusFilter>(filterStatusCombo->currentIndex());
        filter.useDateFilter = filterDateCheck->isChecked();
        filter.startDate = filterStartDate->date();
        filter.endDate = filterEndDate->date();
    }

    std::vector<Activity*> activitiesToDisplay;
    bool isFiltering = false;

    if (filter.keyword.isEmpty() && filter.type == SearchFilter::AllTypes &&
        filter.status == SearchFilter::AnyStatus && !filter.useDateFilter) {
        activitiesToDisplay = manager->getAllActivities();
    } else {
        activitiesToDisplay = manager->searchActivity(filter);
        isFiltering = true;
    }

    int activeViewMode = isFiltering ? 1 : currentView;

    QTreeWidgetItem* todosRoot = mainTree->topLevelItem(2);
    if (todosRoot) {
        todosRoot->setHidden(activeViewMode == 0);
    }

    UiVisitor visitor(mainTree, todoQuickList, activeViewMode, editCallback, deleteCallback);

    for (Activity* act : activitiesToDisplay) {
        act->accept(visitor);
    }


    for (int i = 0; i < mainTree->topLevelItemCount(); ++i) {
        QTreeWidgetItem* categoryItem = mainTree->topLevelItem(i);

        // Riapro le categorie principali (Today, All, ecc.)
        if (expandedCategories.contains(i)) {
            categoryItem->setExpanded(true);
        } else {
            categoryItem->setExpanded(false);
        }

        // Riapro le singole attività per mostrare la descrizione
        for (int j = 0; j < categoryItem->childCount(); ++j) {
            QTreeWidgetItem* activityItem = categoryItem->child(j);
            if (expandedActivities.contains(activityItem->text(0))) {
                activityItem->setExpanded(true);
            }
        }
    }

    mainTree->verticalScrollBar()->setValue(scrollPosition);


    // Ordino i todo a sinistra
    std::vector<QListWidgetItem*> todoItems;

    while (todoQuickList->count() > 0) {
        todoItems.push_back(todoQuickList->takeItem(0));
    }

    std::sort(todoItems.begin(), todoItems.end(), [](QListWidgetItem* a, QListWidgetItem* b) {
        // Puntatori nascosti nel widget per l'ordinamento delle priorità
        ToDo* tA = static_cast<ToDo*>(a->data(Qt::UserRole).value<void*>());
        ToDo* tB = static_cast<ToDo*>(b->data(Qt::UserRole).value<void*>());

        if (tA && tB) {
            if (tA->getPriority() != tB->getPriority()) {
                return tA->getPriority() > tB->getPriority();
            }
            return tA->getTitle() < tB->getTitle();
        }
        return false;
    });

    for (QListWidgetItem* item : todoItems) {
        todoQuickList->addItem(item);
    }

    //Aggiungo le scritte se non ci sono attività o todo

    if (todoQuickList->count() == 0) {
        todoQuickList->setVisible(false);
        lblNoTodos->setVisible(true);
    } else {
        todoQuickList->setVisible(true);
        lblNoTodos->setVisible(false);
    }

    int totalActivities = 0;
    for (int i = 0; i < mainTree->topLevelItemCount(); ++i) {
        if (!mainTree->topLevelItem(i)->isHidden()) {
            totalActivities += mainTree->topLevelItem(i)->childCount();
        }
    }

    if (totalActivities == 0) {
        mainTree->setVisible(false);
        lblNoActivities->setVisible(true);
    } else {
        mainTree->setVisible(true);
        lblNoActivities->setVisible(false);
    }

    btnToday->setProperty("active", currentView == 0);
    btnAll->setProperty("active", currentView == 1);
    btnCompleted->setProperty("active", currentView == 2);

    btnToday->style()->unpolish(btnToday);
    btnToday->style()->polish(btnToday);
    btnAll->style()->unpolish(btnAll);
    btnAll->style()->polish(btnAll);
    btnCompleted->style()->unpolish(btnCompleted);
    btnCompleted->style()->polish(btnCompleted);

    // Riattivo i segnali
    todoQuickList->blockSignals(false);
}

void MainWindow::onTodoToggled(QListWidgetItem* item) {
    // Recupero il puntatore salvato nel widget
    ToDo* t = static_cast<ToDo*>(item->data(Qt::UserRole).value<void*>());

    if (t) {
        bool isChecked = (item->checkState() == Qt::Checked);

        if (isChecked != t->isCompleted()) {
            if(isChecked) {
                isShowingPopup = true;
                QMessageBox::StandardButton reply = QMessageBox::question(this, "Confirm Completion",
                                                                          "Are you sure you want to complete this ToDo?",
                                                                          QMessageBox::Yes | QMessageBox::No);
                isShowingPopup = false;

                if (reply == QMessageBox::Yes) {
                    t->invertCompleted();
                    refreshUI();
                } else {
                    todoQuickList->blockSignals(true);
                    item->setCheckState(Qt::Unchecked);
                    todoQuickList->blockSignals(false);
                }
            } else {
                t->invertCompleted();
                refreshUI();
            }
        }
    }
}


void MainWindow::setupLayout() {

    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QHBoxLayout* mainLayout = new QHBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // Menu per il salvataggio e il caricamento dei file .json
    QMenu* fileMenu = menuBar()->addMenu("File");

    actionSave = fileMenu->addAction("Save As...");
    actionSave->setShortcut(QKeySequence::Save);
    connect(actionSave, &QAction::triggered, this, [this]() {
        QString fileName = QFileDialog::getSaveFileName(this,
                                                        "Save Activity",
                                                        "",
                                                        "JSON Files (*.json);;All Files (*)");
        if (!fileName.isEmpty()) {
            manager->saveToFile(fileName);
        }
    });

    actionLoad = fileMenu->addAction("Load File...");
    actionLoad->setShortcut(QKeySequence::Open);
    connect(actionLoad, &QAction::triggered, this, [this]() {
        QString fileName = QFileDialog::getOpenFileName(this,
                                                        "Load Activity",
                                                        "",
                                                        "JSON Files (*.json);;All Files (*)");
        if (!fileName.isEmpty()) {
            manager->loadFromFile(fileName);
            refreshUI();
        }
    });

    createSidebar();

    line = new QFrame(this);
    line->setFixedWidth(2);
    line->setStyleSheet("QFrame {"
                        "  background-color: rgba(255, 255, 255, 0.08);"
                        "  border-radius: 1px;"
                        "  margin-top: 20px;"
                        "  margin-bottom: 20px;"
                        "}");

    createMainArea();

    mainLayout->addWidget(sidebar, 2);
    mainLayout->addWidget(line);
    mainLayout->addWidget(mainArea, 6);
    mainTree->setWordWrap(true);
    mainTree->header()->setSectionResizeMode(0, QHeaderView::Stretch);

    // Conessione per il check dei todo
    connect(todoQuickList, &QListWidget::itemChanged, this, &MainWindow::onTodoToggled);
}


void MainWindow::createSidebar() {

    sidebar = new QWidget(this);
    sidebar->setMinimumWidth(275);
    QVBoxLayout* sidebarBaseLayout = new QVBoxLayout(sidebar);
    sidebarBaseLayout->setContentsMargins(0, 0, 0, 0);

    QScrollArea* scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    QWidget* scrollContent = new QWidget();
    QVBoxLayout* contentLayout = new QVBoxLayout(scrollContent);
    contentLayout->setContentsMargins(15, 20, 15, 15);
    contentLayout->setSpacing(10);

    btnToday = new QPushButton("Today", this);
    btnAll = new QPushButton("All", this);
    btnCompleted = new QPushButton("Completed", this);

    QString baseTileStyle =
        "QPushButton {"
        "  border-radius: 12px;"
        "  text-align: left;"
        "  padding: 10px 15px;"
        "  font-size: 16px;"
        "  font-weight: 600;"
        "  color: #ffffff;"
        "  border: 2px solid transparent;"
        "}"
        "QPushButton[active='true'] {"
        "  border: 2px solid #ffffff;"
        "}";

    QString todayStyle = baseTileStyle +
                         "QPushButton { background-color: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #60B76D, stop:1 #479A53); }"
                         "QPushButton:hover { background-color: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #6DC77B, stop:1 #60B76D); }";

    QString allStyle = baseTileStyle +
                       "QPushButton { background-color: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #E5736F, stop:1 #D15652); }"
                       "QPushButton:hover { background-color: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #F0827E, stop:1 #E5736F); }";

    QString completedStyle = baseTileStyle +
                             "QPushButton { background-color: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #60A5FA, stop:1 #3B82F6); }"
                             "QPushButton:hover { background-color: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #75B3FF, stop:1 #60A5FA); }";

    btnToday->setStyleSheet(todayStyle);
    btnAll->setStyleSheet(allStyle);
    btnCompleted->setStyleSheet(completedStyle);

    btnToday->setFixedHeight(80);
    btnAll->setFixedHeight(80);
    btnCompleted->setFixedHeight(80);

    QGridLayout* gridLayout = new QGridLayout();
    gridLayout->setSpacing(12);
    gridLayout->setContentsMargins(0, 25, 0, 0);
    gridLayout->addWidget(btnToday, 0, 0);
    gridLayout->addWidget(btnAll, 0, 1);
    gridLayout->addWidget(btnCompleted, 1, 0, 1, 2);

    contentLayout->addLayout(gridLayout);

    contentLayout->addSpacing(20);

    lblToDo = new QLabel("TO DO:", this);
    lblToDo->setStyleSheet("font-weight: bold; font-size: 20px; margin-top: auto;");
    contentLayout->addWidget(lblToDo);

    todoQuickList = new QListWidget(this);
    todoQuickList->setStyleSheet("QListWidget { border: none; background: transparent; } "
                                 "QListWidget::item { padding: 5px; color: white; }"
                                 "QListWidget::indicator { "
                                 "    width: 18px; "
                                 "    height: 18px; "
                                 "} "
                                 "QListWidget::indicator:unchecked { "
                                 "    border: 2px solid #666666; "
                                 "    background-color: #2d2d2d; "
                                 "    border-radius: 4px; "
                                 "} "
                                 );

    todoQuickList->setSelectionMode(QAbstractItemView::NoSelection);
    todoQuickList->setFocusPolicy(Qt::NoFocus);

    contentLayout->addWidget(todoQuickList);
    contentLayout->addStretch();

    lblNoTodos = new QLabel("No todos yet", this);
    lblNoTodos->setStyleSheet("color: #8E8E93; font-size: 15px; font-style: italic; margin-top: 10px;");
    lblNoTodos->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
    lblNoTodos->setVisible(false);
    contentLayout->addWidget(lblNoTodos);

    contentLayout->addStretch();

    scrollArea->setWidget(scrollContent);
    sidebarBaseLayout->addWidget(scrollArea);

    QPushButton* btnLoadJson = new QPushButton(this);
    QPushButton* btnSaveJson = new QPushButton(this);
    btnLoadJson->setIcon(QIcon(":/model/icons/load-button.png"));
    btnSaveJson->setIcon(QIcon(":/model/icons/save-button.png"));
    btnLoadJson->setIconSize(QSize(18, 18));
    btnSaveJson->setIconSize(QSize(18, 18));

    btnLoadJson->setToolTip("Load activity");
    btnSaveJson->setToolTip("Save activity");

    QString iconButtonsStyle =
        "QPushButton { "
        "   background-color: rgba(255, 255, 255, 0.06); "
        "   border: 1px solid rgba(255, 255, 255, 0.03); border-radius: 8px; "
        "   min-width: 36px; max-width: 36px; "
        "   min-height: 36px; max-height: 36px; "
        "} "
        "QPushButton:hover { "
        "   background-color: rgba(255, 255, 255, 0.15); "
        "} "
        "QPushButton:pressed { "
        "   background-color: rgba(255, 255, 255, 0.02); "
        "}";

    btnLoadJson->setStyleSheet(iconButtonsStyle);
    btnSaveJson->setStyleSheet(iconButtonsStyle);

    // Layout bottoni json
    QHBoxLayout* backupLayout = new QHBoxLayout();
    backupLayout->setContentsMargins(20, 0, 20, 20);
    backupLayout->setSpacing(12);
    backupLayout->addStretch();
    backupLayout->addWidget(btnLoadJson);
    backupLayout->addWidget(btnSaveJson);
    backupLayout->addStretch();

    sidebarBaseLayout->addLayout(backupLayout);

    // Collegamenti
    connect(btnSaveJson, &QPushButton::clicked, actionSave, &QAction::trigger);
    connect(btnLoadJson, &QPushButton::clicked, actionLoad, &QAction::trigger);
}


void MainWindow::createMainArea() {

    mainArea = new QWidget(this);
    mainArea->setMinimumWidth(600);

    QVBoxLayout* mainAreaLayout = new QVBoxLayout(mainArea);
    mainAreaLayout->setContentsMargins(30, 20, 30, 20);
    mainAreaLayout->setSpacing(20);

    QHBoxLayout* topBarLayout = new QHBoxLayout();

    btnAdd = new QPushButton("+", this);
    btnAdd->setFixedSize(32, 32);
    btnAdd->setStyleSheet("QPushButton { "
                          "  border: none; "
                          "  border-radius: 16px; "
                          "  background-color: rgba(255, 255, 255, 0.1); "
                          "  color: white; "
                          "  font-weight: bold; "
                          "  font-size: 24px; "
                          "  padding-bottom: 2px; "
                          "} "
                          "QPushButton:hover { background-color: #0a84ff; }");
    btnAdd->setToolTip("Create Activity");

    QMenu* addMenu = new QMenu(this);
    addMenu->setAttribute(Qt::WA_TranslucentBackground);
    addMenu->setWindowFlags(addMenu->windowFlags() | Qt::FramelessWindowHint | Qt::Popup);
    addMenu->setStyleSheet(
        "QMenu {"
        "  background-color: #2d2d2d;"
        "  border: 1px solid rgba(255, 255, 255, 0.1);"
        "  border-radius: 8px;"
        "  padding: 4px;"
        "}"
        "QMenu::item {"
        "  padding: 6px 20px 6px 20px;"
        "  background-color: transparent;"
        "  color: white;"
        "  border-radius: 4px;"
        "}"
        "QMenu::item:selected {"
        "  background-color: #0a84ff;"
        "  color: white;"
        "}"
        );

    QAction* actionEvent = addMenu->addAction("New Event");
    QAction* actionReminder = addMenu->addAction("New Reminder");
    QAction* actionTodo = addMenu->addAction("New ToDo");

    actionEvent->setShortcut(QKeySequence("Ctrl+E"));
    actionEvent->setShortcutContext(Qt::WindowShortcut);
    this->addAction(actionEvent);
    actionReminder->setShortcut(QKeySequence("Ctrl+R"));
    actionReminder->setShortcutContext(Qt::WindowShortcut);
    this->addAction(actionReminder);
    actionTodo->setShortcut(QKeySequence("Ctrl+T"));
    actionTodo->setShortcutContext(Qt::WindowShortcut);
    this->addAction(actionTodo);

    // Posiziono il menu sotto al bottone +
    connect(btnAdd, &QPushButton::clicked, this, [this, addMenu]() {
        int offset_X = (btnAdd->width() - addMenu->sizeHint().width()) / 2;
        int offset_Y = btnAdd->height() + 5;

        QPoint menuPosition = btnAdd->mapToGlobal(QPoint(offset_X, offset_Y));
        addMenu->exec(menuPosition);
    });


    searchBar = new QLineEdit(this);
    searchBar->setPlaceholderText("SEARCH");
    searchBar->setFixedWidth(200);

    btnToggleFilters = new QPushButton(this);
    btnToggleFilters->setIcon(QIcon(":/model/icons/filter-button.png"));
    btnToggleFilters->setIconSize(QSize(18, 18));
    btnToggleFilters->setToolTip("Toggle Filters");
    btnToggleFilters->setFixedSize(32, 32);
    btnToggleFilters->setStyleSheet(
        "QPushButton { "
        "  border: none; "
        "  border-radius: 8px; "
        "  background-color: rgba(255, 255, 255, 0.1); "
        "} "
        "QPushButton:hover { background-color: #0a84ff; }"
        );

    topBarLayout->addStretch();
    topBarLayout->addWidget(btnAdd);
    topBarLayout->addWidget(searchBar);
    topBarLayout->addWidget(btnToggleFilters);

    mainAreaLayout->addLayout(topBarLayout);

    searchBar->setStyleSheet("QLineEdit { "
                             "  border: none; "
                             "  border-radius: 8px; "
                             "  padding: 6px 12px; "
                             "  background: rgba(255, 255, 255, 0.1); "
                             "  color: white; "
                             "  padding-left: 5px; "
                             "} "
                             "QLineEdit:focus { background: rgba(255, 255, 255, 0.15); }");

    // Connessione ricerca testuale in tempo reale
    connect(searchBar, &QLineEdit::textChanged, this, &MainWindow::refreshUI);

    filterPanel = new QFrame(this);
    filterPanel->setStyleSheet(
        "QFrame {"
        "  background-color: #252525;"
        "  border: 1px solid rgba(255, 255, 255, 0.08);"
        "  border-radius: 8px;"
        "}"
        "QLabel {"
        "  color: #8E8E93;"
        "  font-size: 11px;"
        "  font-weight: 800;"
        "  letter-spacing: 1px;"
        "  border: none;"
        "}"
        "QComboBox, QDateEdit {"
        "  background-color: rgba(0, 0, 0, 0.2);"
        "  color: white;"
        "  border: 1px solid rgba(255, 255, 255, 0.1);"
        "  border-radius: 6px;"
        "  padding: 6px 10px;"
        "  font-size: 13px;"
        "}"
        "QComboBox::drop-down { border: none; }"
        "QCheckBox {"
        "  color: white;"
        "  font-weight: 600;"
        "  font-size: 13px;"
        "  border: none;"
        "}"
        "QCheckBox::indicator {"
        "  width: 18px;"
        "  height: 18px;"
        "}"
        "QCheckBox::indicator:unchecked {"
        " border: 2px solid #666666; "
        "    background-color: #2d2d2d; "
        "    border-radius: 4px; "
        "}"
        );


    QVBoxLayout* filterMainLayout = new QVBoxLayout(filterPanel);
    filterMainLayout->setContentsMargins(15, 15, 15, 15);
    filterMainLayout->setSpacing(15);

    filterTypeCombo = new QComboBox(this);
    filterTypeCombo->addItems({"All Types", "Events", "Reminders", "ToDos"});

    filterStatusCombo = new QComboBox(this);
    filterStatusCombo->addItems({"Any Status", "Completed", "Pending"});

    filterDateCheck = new QCheckBox(" Filter by Date:", this);
    filterStartDate = new QDateEdit(QDate::currentDate(), this);
    filterStartDate->setCalendarPopup(true);
    filterEndDate = new QDateEdit(QDate::currentDate().addDays(7), this);
    filterEndDate->setCalendarPopup(true);

    btnResetFilters = new QPushButton("Clean Filters", this);
    btnResetFilters->setCursor(Qt::PointingHandCursor);
    btnResetFilters->setStyleSheet(
        "QPushButton { "
        "  background-color: transparent; "
        "  color: #E5736F; "
        "  font-weight: 800; "
        "  font-size: 11px; "
        "  border: 1px solid #E5736F; "
        "  border-radius: 4px; "
        "  padding: 4px 12px; "
        "} "
        "QPushButton:hover { background-color: rgba(229, 115, 111, 0.15); }"
        );

    QHBoxLayout* row1 = new QHBoxLayout();
    row1->addWidget(new QLabel("TYPE", this));
    row1->addWidget(filterTypeCombo);
    row1->addSpacing(30);
    row1->addWidget(new QLabel("STATUS", this));
    row1->addWidget(filterStatusCombo);
    row1->addStretch();

    QHBoxLayout* row2 = new QHBoxLayout();
    row2->addWidget(filterDateCheck);
    row2->addWidget(filterStartDate);
    QLabel* lblTo = new QLabel("TO", this);
    lblTo->setAlignment(Qt::AlignCenter);
    row2->addWidget(lblTo);
    row2->addWidget(filterEndDate);
    row2->addStretch();
    row2->addWidget(btnResetFilters);

    filterMainLayout->addLayout(row1);
    filterMainLayout->addLayout(row2);

    filterPanel->setVisible(false);
    mainAreaLayout->addWidget(filterPanel);

    connect(btnResetFilters, &QPushButton::clicked, this, [this]() {
        searchBar->blockSignals(true);
        filterTypeCombo->blockSignals(true);
        filterStatusCombo->blockSignals(true);
        filterDateCheck->blockSignals(true);
        filterStartDate->blockSignals(true);
        filterEndDate->blockSignals(true);

        searchBar->clear();
        filterTypeCombo->setCurrentIndex(0);
        filterStatusCombo->setCurrentIndex(0);
        filterDateCheck->setChecked(false);
        filterStartDate->setDate(QDate::currentDate());
        filterEndDate->setDate(QDate::currentDate().addDays(7));

        searchBar->blockSignals(false);
        filterTypeCombo->blockSignals(false);
        filterStatusCombo->blockSignals(false);
        filterDateCheck->blockSignals(false);
        filterStartDate->blockSignals(false);
        filterEndDate->blockSignals(false);

        refreshUI();
    });

    filterPanel->setVisible(false);
    mainAreaLayout->addWidget(filterPanel);

    connect(btnToggleFilters, &QPushButton::clicked, this, [this]() {
        filterPanel->setVisible(!filterPanel->isVisible());
    });
    connect(filterTypeCombo, &QComboBox::currentIndexChanged, this, &MainWindow::refreshUI);
    connect(filterStatusCombo, &QComboBox::currentIndexChanged, this, &MainWindow::refreshUI);
    connect(filterDateCheck, &QCheckBox::stateChanged, this, &MainWindow::refreshUI);
    connect(filterStartDate, &QDateEdit::dateChanged, this, &MainWindow::refreshUI);
    connect(filterEndDate, &QDateEdit::dateChanged, this, &MainWindow::refreshUI);

    sectionHeader = new QLabel("Today", this);
    sectionHeader->setStyleSheet("font-size: 52px; font-weight: bold; margin-bottom: 10px;");

    mainTree = new QTreeWidget(this);
    mainTree->setHeaderHidden(true);
    mainTree->setStyleSheet("QTreeWidget { "
                            "  border: none; "
                            "  background-color: transparent; "
                            "  font-size: 15px; "
                            "} "
                            "QTreeWidget::item { "
                            "  padding: 0px 0px; "
                            "} "
                            "QTreeWidget::item:selected { "
                            "  background-color: transparent; "
                            "}");
    mainTree->setColumnCount(2);
    mainTree->setWordWrap(true);
    mainTree->header()->setStretchLastSection(false);
    mainTree->header()->setSectionResizeMode(0, QHeaderView::Stretch);
    mainTree->header()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    mainTree->setSelectionMode(QAbstractItemView::NoSelection);
    mainTree->setFocusPolicy(Qt::NoFocus);
    mainTree->setIconSize(QSize(16, 16));


    QTreeWidgetItem* categoryEvents = new QTreeWidgetItem(mainTree);
    categoryEvents->setText(0, "Events");
    QFont categoryFont = categoryEvents->font(0);
    categoryFont.setBold(true);
    categoryFont.setPointSize(16);
    categoryEvents->setFont(0, categoryFont);
    categoryEvents->setExpanded(true);
    categoryEvents->setSizeHint(0, QSize(0, 50));

    QTreeWidgetItem* categoryReminders = new QTreeWidgetItem(mainTree);
    categoryReminders->setText(0, "Reminders");
    categoryReminders->setFont(0, categoryFont);
    categoryReminders->setExpanded(true);
    categoryReminders->setSizeHint(0, QSize(0, 50));

    QTreeWidgetItem* categoryTodos = new QTreeWidgetItem(mainTree);
    categoryTodos->setText(0, "TO DO");
    categoryTodos->setFont(0, categoryFont);
    categoryTodos->setExpanded(true);
    categoryTodos->setSizeHint(0, QSize(0, 50));

    mainAreaLayout->addWidget(sectionHeader);
    mainAreaLayout->addWidget(mainTree);

    lblNoActivities = new QLabel("No activities yet", this);
    lblNoActivities->setStyleSheet("color: #8E8E93; font-size: 20px; font-style: italic; margin-top: 60px;");
    lblNoActivities->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    lblNoActivities->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
    lblNoActivities->setVisible(false);
    mainAreaLayout->addWidget(lblNoActivities);

    formSeparator = new QFrame(this);
    formSeparator->setFixedHeight(2);
    formSeparator->setStyleSheet(
        "QFrame {"
        "  background-color: rgba(255, 255, 255, 0.08);"
        "  border-radius: 1px;"
        "  margin-left: 30px;"
        "  margin-right: 30px;"
        "  margin-top: 5px;"
        "  margin-bottom: 5px;"
        "}"
        );

    formSeparator->setVisible(false);
    mainAreaLayout->addWidget(formSeparator);

    formContainer = new QStackedWidget(this);

    formEvent = new AddEventDialog(this);
    formReminder = new AddReminderDialog(this);
    formTodo = new AddTodoDialog(this);

    formContainer->addWidget(formEvent);
    formContainer->addWidget(formReminder);
    formContainer->addWidget(formTodo);

    formContainer->setStyleSheet(
        "QLineEdit, QTextEdit, QDateTimeEdit, QComboBox, QSpinBox {"
        "  background-color: rgba(255, 255, 255, 0.05);"
        "  border: 1px solid rgba(255, 255, 255, 0.03);"
        "  border-radius: 6px;"
        "  padding: 6px 10px;"
        "  color: white;"
        "  font-size: 14px;"
        "}"

        "QLineEdit:focus, QTextEdit:focus, QDateTimeEdit:focus, QComboBox:focus, QSpinBox:focus {"
        "  background-color: rgba(255, 255, 255, 0.08);"
        "  border: 1px solid #0a84ff;"
        "}"

        "QLabel {"
        "  color: #8e8e93;"
        "  font-size: 13px;"
        "  font-weight: 600;"
        "}"

        "QPushButton {"
        "  border: none;"
        "  border-radius: 6px;"
        "  padding: 6px 16px;"
        "  font-size: 13px;"
        "  font-weight: 600;"
        "  background-color: rgba(255, 255, 255, 0.1);"
        "  color: white;"
        "}"
        "QPushButton:hover {"
        "  background-color: rgba(255, 255, 255, 0.15);"
        "}"
        );

    formContainer->setVisible(false);

    mainAreaLayout->addWidget(formContainer);

    // Connessioni per la logica di creazione delle attività

    connect(actionEvent, &QAction::triggered, this, [this]() {
        formContainer->setCurrentIndex(0);
        formContainer->setVisible(true);
        formSeparator->setVisible(true);
        formEvent->focusTitle();
    });

    connect(actionReminder, &QAction::triggered, this, [this]() {
        formContainer->setCurrentIndex(1); // Mostra Reminder
        formContainer->setVisible(true);
        formSeparator->setVisible(true);
        formReminder->focusTitle();
    });

    connect(actionTodo, &QAction::triggered, this, [this]() {
        formContainer->setCurrentIndex(2); // Mostra ToDo
        formContainer->setVisible(true);
        formSeparator->setVisible(true);
        formTodo->focusTitle();
    });


    // Connessioni per la logica di salvataggio, chiusura e modifica delle attività

    connect(formEvent, &AddEventDialog::eventSaved, this, [this](Event* e) {
        manager->addActivity(e);
        refreshUI();
        formContainer->setVisible(false);
        formSeparator->setVisible(false);
    });
    connect(formEvent, &AddEventDialog::formCancelled, this, [this]() {
        formContainer->setVisible(false);
        formSeparator->setVisible(false);
    });
    connect(formEvent, &AddEventDialog::eventEdited, this, [this]() {
        refreshUI();
        formContainer->setVisible(false);
        formSeparator->setVisible(false);
    });

    connect(formReminder, &AddReminderDialog::reminderSaved, this, [this](Reminder* r) {
        manager->addActivity(r);
        refreshUI();
        formContainer->setVisible(false);
        formSeparator->setVisible(false);
    });
    connect(formReminder, &AddReminderDialog::formCancelled, this, [this]() {
        formContainer->setVisible(false);
        formSeparator->setVisible(false);
    });
    connect(formReminder, &AddReminderDialog::reminderEdited, this, [this]() {
        refreshUI();
        formContainer->setVisible(false);
        formSeparator->setVisible(false);
    });

    connect(formTodo, &AddTodoDialog::todoSaved, this, [this](ToDo* t) {
        manager->addActivity(t);
        refreshUI();
        formContainer->setVisible(false);
        formSeparator->setVisible(false);
    });
    connect(formTodo, &AddTodoDialog::formCancelled, this, [this]() {
        formContainer->setVisible(false);
        formSeparator->setVisible(false);
    });
    connect(formTodo, &AddTodoDialog::todoEdited, this, [this]() {
        refreshUI();
        formContainer->setVisible(false);
        formSeparator->setVisible(false);
    });

}

void MainWindow::openEditPanel(Activity* a) {
    if (!a) return;

    formContainer->setVisible(true);
    formSeparator->setVisible(true);

    FormEditVisitor visitor(formEvent, formReminder, formTodo, formContainer);

    a->accept(visitor);
}


void MainWindow::checkReminders() {

    if (!manager || isShowingPopup) return;

    for (Activity* act : manager->getAllActivities()) {

        if (Reminder* rem = dynamic_cast<Reminder*>(act)) {

            if (!rem->isCompleted() && rem->hasToRing()) {

                isShowingPopup = true;

                QMessageBox msgBox(this);
                msgBox.setWindowTitle("Reminder!");

                msgBox.setText("<span style='font-size: 16px; font-weight: 600; color: white;'>" + rem->getTitle() + "</span>");
                msgBox.setInformativeText("<span style='font-size: 13px; color: #8e8e93;'>" + rem->getDescription() + "</span>");
                msgBox.setStyleSheet(
                    "QMessageBox {"
                    "  background-color: rgba(255, 69, 58, 0.15);"
                    "  border: 1px solid rgba(255, 69, 58, 0.3);"
                    "  border-radius: 10px;"
                    "}"
                    "QPushButton {"
                    "  background-color: rgba(255, 255, 255, 0.1);"
                    "  color: white;"
                    "  border: none;"
                    "  border-radius: 6px;"
                    "  padding: 6px 16px;"
                    "}"
                    );
                // Creo il lampeggio del popup
                QTimer flashTimer(&msgBox);
                connect(&flashTimer, &QTimer::timeout, &msgBox, [&msgBox]() {
                    static bool isRed = false;
                    isRed = !isRed;
                    if (isRed) {
                        // Stato acceso
                        msgBox.setStyleSheet(
                            "QMessageBox {"
                            "  background-color: rgba(255, 69, 58, 0.15);"
                            "  border: 1px solid rgba(255, 69, 58, 0.3);"
                            "  border-radius: 10px;"
                            "}"
                            "QPushButton {"
                            "  background-color: rgba(255, 255, 255, 0.1);"
                            "  color: white;"
                            "  border: none;"
                            "  border-radius: 6px;"
                            "  padding: 6px 16px;"
                            "}"
                            );
                    } else {
                        // Stato spento
                        msgBox.setStyleSheet(
                            "QMessageBox {"
                            "  background-color: #2d2d2d;"
                            "  border: 1px solid rgba(255, 255, 255, 0.1);"
                            "  border-radius: 10px;"
                            "}"
                            "QPushButton {"
                            "  background-color: rgba(255, 255, 255, 0.1);"
                            "  color: white;"
                            "  border: none;"
                            "  border-radius: 6px;"
                            "  padding: 6px 16px;"
                            "}"
                            );
                    }
                });

                flashTimer.start(600);

                QApplication::alert(this);

                QPushButton *snoozeBtn = msgBox.addButton("Snooze (10 min)", QMessageBox::ActionRole);
                QPushButton *completeBtn = msgBox.addButton("Completed", QMessageBox::AcceptRole);

                completeBtn->setStyleSheet("background-color: #0a84ff; color: white; font-weight: bold;");

                msgBox.exec();

                if (msgBox.clickedButton() == snoozeBtn) {
                    rem->snooze(10);
                }
                else if (msgBox.clickedButton() == completeBtn) {
                    rem->confirmRead();
                }

                isShowingPopup = false;

                refreshUI();
            }
        }
    }
}


void MainWindow::scheduleNextMinuteRefresh() {
    QTime now = QTime::currentTime();

    // Calcolo millisecondi mancanti alla fine del minuto corrente
    int msToNextMinute = (60 - now.second()) * 1000 - now.msec();

    QTimer::singleShot(msToNextMinute, this, [this]() {

        refreshUI();
        checkReminders();

        scheduleNextMinuteRefresh();
    });
}
