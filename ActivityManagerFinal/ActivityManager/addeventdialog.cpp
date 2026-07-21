#include "addeventdialog.h"

AddEventDialog::AddEventDialog(QWidget *parent) : QWidget(parent) {

    setWindowTitle("New Event");
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

    startEdit = new QDateTimeEdit(QDateTime::currentDateTime().addSecs(600), this);
    startEdit->setCalendarPopup(true);
    endEdit = new QDateTimeEdit(QDateTime::currentDateTime().addSecs(3600), this);
    endEdit->setCalendarPopup(true);

    locInput = new QLineEdit(this);
    locInput->setMaxLength(30);

    formLayout->addRow("Title*:", titleInput);
    formLayout->addRow("Description:", descInput);
    formLayout->addRow("Start date:", startEdit);
    formLayout->addRow("End date:", endEdit);
    formLayout->addRow("Location:", locInput);

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

        QDateTime start = startEdit->dateTime();
        QDateTime end = endEdit->dateTime();
        start.setTime(QTime(start.time().hour(), start.time().minute(), 0));
        end.setTime(QTime(end.time().hour(), end.time().minute(), 0));

        if (start <= QDateTime::currentDateTime()) {
            QMessageBox::warning(this, "Invalid Date", "The start date cannot be in the past.\nPlease choose a future date or time.");
            return;
        }

        if (start > end) {
            QMessageBox::warning(this, "Invalid Date", "The end date cannot be earlier than the start date.");
            return;
        }

        try {
            if (eventToEdit != nullptr) {
                eventToEdit->setTitle(titleInput->text());
                eventToEdit->setDescription(descInput->toPlainText());
                eventToEdit->setStartDate(start);
                eventToEdit->setEndDate(end);
                eventToEdit->setLocation(locInput->text());

                emit eventEdited();
            } else {
                Event* newEvent = new Event(
                    titleInput->text(),
                    descInput->toPlainText(),
                    start,
                    end,
                    locInput->text()
                    );

                emit eventSaved(newEvent);
            }

            clearForm();

        } catch (const std::invalid_argument& e) {
            QMessageBox::warning(this, "Invalid Dates", e.what());
        }
    });


    connect(btnCancel, &QPushButton::clicked, this, [this]() {

        emit formCancelled();
        clearForm();

    });
}


void AddEventDialog::clearForm() {
    titleInput->clear();
    titleInput->setReadOnly(false);

    descInput->clear();
    descInput->setReadOnly(false);

    locInput->clear();
    locInput->setReadOnly(false);

    // Rimetto la data e ora attuali + 10 minuti per l'inizio e + 60 minuti per la fine
    startEdit->setDateTime(QDateTime::currentDateTime().addSecs(600));
    startEdit->setEnabled(true);

    endEdit->setDateTime(QDateTime::currentDateTime().addSecs(3600));
    endEdit->setEnabled(true);

    btnOk->setEnabled(false);

    eventToEdit = nullptr;
    btnOk->setText("Save");
}

void AddEventDialog::loadEvent(Event* e) {
    eventToEdit = e;

    titleInput->setText(e->getTitle());
    descInput->setText(e->getDescription());
    startEdit->setDateTime(e->getStartDate());
    endEdit->setDateTime(e->getEndDate());
    locInput->setText(e->getLocation());

    btnOk->setText("Update");

    if (e->isCompleted()) {
        titleInput->setReadOnly(true);
        descInput->setReadOnly(true);
        locInput->setReadOnly(true);
        startEdit->setEnabled(false);
        endEdit->setEnabled(false);

        btnOk->setEnabled(false);
    } else {
        titleInput->setReadOnly(false);
        descInput->setReadOnly(false);
        locInput->setReadOnly(false);
        startEdit->setEnabled(true);
        endEdit->setEnabled(true);

        btnOk->setEnabled(true);
        titleInput->setFocus();
    }
}

void AddEventDialog::focusTitle() {
    QTimer::singleShot(0, this, [this]() {
        titleInput->setFocus();
    });
}
