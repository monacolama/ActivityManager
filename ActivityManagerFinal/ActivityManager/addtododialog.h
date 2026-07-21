#ifndef ADDTODODIALOG_H
#define ADDTODODIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QTextEdit>
#include <QComboBox>
#include <QPushButton>
#include <QWidget>
#include <QTimer>
#include <QShortcut>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>

#include "ToDo.h"

class AddTodoDialog : public QWidget {
    Q_OBJECT

public:
    explicit AddTodoDialog(QWidget *parent = nullptr);
    void focusTitle();
    void loadTodo(ToDo* t);
    void clearForm();

private:
    QLineEdit* titleInput;
    QTextEdit* descInput;
    QComboBox* priorityCombo;

    QPushButton* btnOk;
    QPushButton* btnCancel;

    ToDo* todoToEdit = nullptr;

signals:
    void todoSaved(ToDo* t);
    void formCancelled();
    void todoEdited();
};

#endif // ADDTODODIALOG_H
