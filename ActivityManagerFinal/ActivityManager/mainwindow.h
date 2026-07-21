#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFrame>
#include <QPushButton>
#include <QListWidget>
#include <QLabel>
#include <QLineEdit>
#include <QScrollArea>
#include <QTreeWidget>
#include <QInputDialog>
#include <QMenu>
#include <QAction>
#include <QInputDialog>
#include <QDate>
#include <QDateTime>
#include <QTimer>
#include <QHeaderView>
#include <QStackedWidget>
#include <QIcon>
#include <QSize>
#include <QMessageBox>
#include <QTreeWidgetItemIterator>
#include <QFileDialog>
#include <QMenuBar>
#include <QScrollBar>
#include <QSet>
#include <QApplication>
#include <QComboBox>
#include <QCheckBox>
#include <QDateEdit>

#include "ActivityManager.h"
#include "ConstitemVisitor.h"
#include "ItemVisitor.h"
#include "addeventdialog.h"
#include "addreminderdialog.h"
#include "addtododialog.h"
#include "uivisitor.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    ActivityManager* manager;

    QWidget* sidebar;
    QWidget* mainArea;
    QFrame* line;

    QPushButton* btnToday;
    QPushButton* btnAll;
    QPushButton* btnCompleted;

    QLabel* lblToDo;
    QListWidget* todoQuickList;

    QLineEdit* searchBar;
    QPushButton* btnToggleFilters;
    QFrame* filterPanel;
    QComboBox* filterTypeCombo;
    QComboBox* filterStatusCombo;
    QCheckBox* filterDateCheck;
    QDateEdit* filterStartDate;
    QDateEdit* filterEndDate;
    QPushButton* btnResetFilters;

    QPushButton* btnAdd;
    QLabel* lblMainTitle;

    QAction* actionSave;
    QAction* actionLoad;

    QTreeWidget* mainTree;
    QLabel* sectionHeader;

    int currentView;
    bool isShowingPopup;


    void refreshUI();
    void setupLayout();
    void createSidebar();
    void createMainArea();
    void onTodoToggled(QListWidgetItem*);

    QStackedWidget* formContainer;
    AddEventDialog* formEvent;
    AddReminderDialog* formReminder;
    AddTodoDialog* formTodo;
    QFrame* formSeparator;

    void openEditPanel(Activity* a);

    QLabel* lblNoTodos;
    QLabel* lblNoActivities;

private slots:

    void checkReminders();
    void scheduleNextMinuteRefresh();
};
#endif // MAINWINDOW_H
