#include "addtododialog.h"

AddTodoDialog::AddTodoDialog(QWidget *parent) : QWidget(parent) {

    setWindowTitle("New ToDo");
    setMinimumHeight(300);
    setMaximumHeight(300);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    QFormLayout* formLayout = new QFormLayout();

    titleInput = new QLineEdit(this);
    titleInput->setMaxLength(50);

    descInput = new QTextEdit(this);
    descInput->setMaximumHeight(80);

    connect(descInput, &QTextEdit::textChanged, this, [this]() {
        int maxChar = 500;
        QString descText = descInput->toPlainText();

        if (descText.length() > maxChar) {

            descInput->setPlainText(descText.left(maxChar));

            QTextCursor cursor = descInput->textCursor();
            cursor.movePosition(QTextCursor::End);
            descInput->setTextCursor(cursor);
        }
    });

    priorityCombo = new QComboBox(this);
    priorityCombo->addItems({"None", "Low", "Medium", "High"});

    formLayout->addRow("Title*:", titleInput);
    formLayout->addRow("Description:", descInput);
    formLayout->addRow("Priority:", priorityCombo);

    QHBoxLayout* leftAlignLayout = new QHBoxLayout();
    leftAlignLayout->addLayout(formLayout);
    leftAlignLayout->addStretch();

    mainLayout->addLayout(leftAlignLayout);

    QHBoxLayout* btnLayout = new QHBoxLayout();
    btnOk = new QPushButton("Save", this);
    btnCancel = new QPushButton("Cancel", this);
    QShortcut* shortcutInvio = new QShortcut(QKeySequence(Qt::Key_Return), this);
    connect(shortcutInvio, &QShortcut::activated, btnOk, &QPushButton::click);
    QShortcut* shortcutInvioNum = new QShortcut(QKeySequence(Qt::Key_Enter), this);
    connect(shortcutInvioNum, &QShortcut::activated, btnOk, &QPushButton::click);
    QShortcut* shortcutEsc = new QShortcut(QKeySequence(Qt::Key_Escape), this);
    connect(shortcutEsc, &QShortcut::activated, btnCancel, &QPushButton::click);

    btnOk->setStyleSheet(
        "QPushButton:disabled { "
        "   background-color: rgba(255, 255, 255, 0.05); color: #666666; "
        "   border: 1px solid transparent; border-radius: 6px; padding: 6px 16px; "
        "   font-weight: 600; font-size: 13px;"
        "} "
        "QPushButton:enabled { "
        "   background-color: #0a84ff; color: white; "
        "   border: 1px solid transparent; border-radius: 6px; padding: 6px 16px; "
        "   font-weight: 600; font-size: 13px;"
        "}"
        "QPushButton:enabled:hover { "
        "   background-color: #007aff;"
        "}"
        );

    btnCancel->setStyleSheet(
        "QPushButton { "
        "   background-color: rgba(255, 255, 255, 0.1); color: white; "
        "   border: 1px solid transparent; border-radius: 6px; padding: 6px 16px; "
        "   font-weight: 600; font-size: 13px;"
        "} "
        "QPushButton:hover { "
        "   background-color: rgba(255, 255, 255, 0.15);"
        "}"
        );

    btnOk->setEnabled(false);

    connect(titleInput, &QLineEdit::textChanged, this, [this](const QString& text) {
        btnOk->setEnabled(!text.trimmed().isEmpty());
    });

    btnLayout->addStretch();
    btnLayout->addWidget(btnCancel);
    btnLayout->addWidget(btnOk);
    mainLayout->addLayout(btnLayout);

    connect(btnOk, &QPushButton::clicked, this, [this]() {

        ToDo::priorityOfTodo prioritySelected = static_cast<ToDo::priorityOfTodo>(priorityCombo->currentIndex());

        if (todoToEdit != nullptr) {
            todoToEdit->setTitle(titleInput->text());
            todoToEdit->setDescription(descInput->toPlainText());
            todoToEdit->setPriority(prioritySelected);

            emit todoEdited();
        } else {
            ToDo* nuovoTodo = new ToDo(
                titleInput->text(),
                descInput->toPlainText(),
                prioritySelected
                );

            emit todoSaved(nuovoTodo);
        }

        clearForm();

    });

    connect(btnCancel, &QPushButton::clicked, this, [this]() {

        emit formCancelled();
        clearForm();
    });
}



void AddTodoDialog::clearForm() {
    titleInput->clear();
    titleInput->setReadOnly(false);

    descInput->clear();
    descInput->setReadOnly(false);

    priorityCombo->setCurrentIndex(0);
    priorityCombo->setEnabled(true);

    btnOk->setEnabled(false);

    todoToEdit = nullptr;
    btnOk->setText("Save");
}


void AddTodoDialog::loadTodo(ToDo* t) {
    todoToEdit = t;

    titleInput->setText(t->getTitle());
    descInput->setText(t->getDescription());
    priorityCombo->setCurrentIndex(static_cast<int>(t->getPriority()));

    btnOk->setText("Update");

    if (t->isCompleted()) {
        titleInput->setReadOnly(true);
        descInput->setReadOnly(true);
        priorityCombo->setEnabled(false);

        btnOk->setEnabled(false);
    } else {
        titleInput->setReadOnly(false);
        descInput->setReadOnly(false);
        priorityCombo->setEnabled(true);

        btnOk->setEnabled(true);
        titleInput->setFocus();
    }
}

void AddTodoDialog::focusTitle() {
    QTimer::singleShot(0, this, [this]() {
        titleInput->setFocus();
    });
}
