#ifndef ADDREMINDERDIALOG_H
#define ADDREMINDERDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QTextEdit>
#include <QDateTimeEdit>
#include <QComboBox>
#include <QPushButton>
#include <QWidget>
#include <QTimer>
#include <QShortcut>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <stdexcept>

#include "Reminder.h"

class AddReminderDialog : public QWidget {
    Q_OBJECT

public:
    explicit AddReminderDialog(QWidget *parent = nullptr);
    void focusTitle();
    void loadReminder(Reminder* r);
    void clearForm();

private:
    QLineEdit* titleInput;
    QTextEdit* descInput;
    QDateTimeEdit* dateEdit;
    QComboBox* freqCombo;

    QPushButton* btnOk;
    QPushButton* btnCancel;

    Reminder* reminderToEdit = nullptr;

signals:
    void reminderSaved(Reminder* r);
    void formCancelled();
    void reminderEdited();
};

#endif // ADDREMINDERDIALOG_H
