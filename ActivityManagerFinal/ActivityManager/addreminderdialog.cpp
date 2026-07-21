#include "addreminderdialog.h"

AddReminderDialog::AddReminderDialog(QWidget *parent) : QWidget(parent) {

    setWindowTitle("New Reminder");
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

    dateEdit = new QDateTimeEdit(QDateTime::currentDateTime().addSecs(600), this);
    dateEdit->setCalendarPopup(true);

    freqCombo = new QComboBox(this);
    freqCombo->addItems({"Never", "Daily", "Weekly", "Monthly", "Yearly"});

    formLayout->addRow("Title*:", titleInput);
    formLayout->addRow("Description:", descInput);
    formLayout->addRow("Notify:", dateEdit);
    formLayout->addRow("Repetition:", freqCombo);

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

        // La data scelta dall'utente viene salvata in una variabile e
        // vengono azzerati i secondi e millisecondi per far iniziare
        // l'attività alle 00
        QDateTime chosenDate = dateEdit->dateTime();
        chosenDate.setTime(QTime(chosenDate.time().hour(), chosenDate.time().minute(), 0));

        if (chosenDate <= QDateTime::currentDateTime()) {
            QMessageBox::warning(this, "Reminder Error", "You cannot schedule a reminder in the past.\nPlease choose a future date or time.");
            return;
        }

        try {
            Reminder::frequencyOfRepetition frequencySelected = static_cast<Reminder::frequencyOfRepetition>(freqCombo->currentIndex());

            if (reminderToEdit != nullptr) {
                reminderToEdit->setTitle(titleInput->text());
                reminderToEdit->setDescription(descInput->toPlainText());
                reminderToEdit->setDateOfNotification(chosenDate);
                reminderToEdit->setFrequency(frequencySelected);

                emit reminderEdited();
            } else {
                Reminder* nuovoReminder = new Reminder(
                    titleInput->text(),
                    descInput->toPlainText(),
                    chosenDate,
                    frequencySelected
                    );

                emit reminderSaved(nuovoReminder);
            }

            clearForm();

        } catch (const std::invalid_argument& r) {
            QMessageBox::warning(this, "Reminder Error", r.what());
        }
    });


    connect(btnCancel, &QPushButton::clicked, this, [this]() {

        emit formCancelled();
        clearForm();
    });
}


void AddReminderDialog::clearForm() {
    titleInput->clear();
    titleInput->setReadOnly(false);

    descInput->clear();
    descInput->setReadOnly(false);

    // Rimetto la data e ora attuali + 10 minuti
    dateEdit->setDateTime(QDateTime::currentDateTime().addSecs(600));
    dateEdit->setEnabled(true);

    freqCombo->setCurrentIndex(0);
    freqCombo->setEnabled(true);

    btnOk->setEnabled(false);

    reminderToEdit = nullptr;
    btnOk->setText("Save");
}

void AddReminderDialog::loadReminder(Reminder* r) {
    reminderToEdit = r;

    titleInput->setText(r->getTitle());
    descInput->setText(r->getDescription());
    dateEdit->setDateTime(r->getDateOfNotification());

    freqCombo->setCurrentIndex(static_cast<int>(r->getFrequency()));

    btnOk->setText("Update");

    if (r->isCompleted()) {
        titleInput->setReadOnly(true);
        descInput->setReadOnly(true);
        dateEdit->setEnabled(false);
        freqCombo->setEnabled(false);

        btnOk->setEnabled(false);
    } else {
        titleInput->setReadOnly(false);
        descInput->setReadOnly(false);
        dateEdit->setEnabled(true);
        freqCombo->setEnabled(true);

        btnOk->setEnabled(true);
        titleInput->setFocus();
    }
}

void AddReminderDialog::focusTitle() {
    QTimer::singleShot(0, this, [this]() {
        titleInput->setFocus();
    });
}
