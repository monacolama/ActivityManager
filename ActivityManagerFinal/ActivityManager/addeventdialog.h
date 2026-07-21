#ifndef ADDEVENTDIALOG_H
#define ADDEVENTDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QDateTimeEdit>
#include <QTextEdit>
#include <QPushButton>
#include <QWidget>
#include <QTimer>
#include <QShortcut>
#include <QMessageBox>
#include <QDateTime>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <stdexcept>

#include "Event.h"

class AddEventDialog : public QWidget{
    Q_OBJECT

public:
    explicit AddEventDialog(QWidget *parent = nullptr);
    void focusTitle();
    void loadEvent(Event* e);
    void clearForm();

private:

    QLineEdit* titleInput;
    QTextEdit* descInput;
    QDateTimeEdit* startEdit;
    QDateTimeEdit* endEdit;
    QLineEdit* locInput;

    QPushButton* btnOk;
    QPushButton* btnCancel;

    Event* eventToEdit = nullptr;

signals:
    void eventSaved(Event* e);
    void formCancelled();
    void eventEdited();
};

#endif // ADDEVENTDIALOG_H
