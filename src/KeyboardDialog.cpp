#include "KeyboardDialog.h"
#include "ui_KeyboardDialog.h"

#include <QDebug>

KeyboardDialog::KeyboardDialog(const QString &text, Mode mode, QWidget *parent)
    : InputProviderDialog(parent), ui(new Ui::KeyboardDialog), shifted(false) {
    ui->setupUi(this);

    alphaVerticalLayout = new QVBoxLayout(this);
    numVerticalLayout = new QVBoxLayout(this);
    alphaVerticalLayout->setContentsMargins(0, 3, 0, 9);
    numVerticalLayout->setContentsMargins(0, 3, 0, 9);

    label = new QLabel(this);
    display = new QLineEdit(this);
    QFont font;
    font.setPointSize(16);
    label->setText(text);
    label->setFont(font);
    display->setFont(font);
    if (mode == Password) display->setEchoMode(QLineEdit::Password);
    alphaVerticalLayout->addWidget(label);
    alphaVerticalLayout->addWidget(display);

    ui->alphaGrid->addLayout(alphaVerticalLayout, 0, 0, 2, -1);
    ui->numGrid->addLayout(numVerticalLayout, 0, 0, 2, -1);

    setupSpecialButtons();
    setupAlphaButtons();
    setupNumButtons();
    ui->numWidget->hide();
}

KeyboardDialog::~KeyboardDialog() { delete ui; }

void KeyboardDialog::setInput(const QString &input) { display->setText(input); }

QString KeyboardDialog::input() const { return display->text(); }

void KeyboardDialog::symbolPressed() {
    KeyButton *button = qobject_cast<KeyButton *>(sender());
    display->insert(button->text());

    if (shifted) shiftPressed();
}

void KeyboardDialog::spacePressed() { display->insert(" "); }

void KeyboardDialog::shiftPressed() {
    for (int i = 0; i < 26; ++i)
        alphaButtons[i]->switchLabel();
    shifted = !shifted;
}

void KeyboardDialog::delPressed() { display->backspace(); }

void KeyboardDialog::numPressed() {
    if (shifted) shiftPressed();

    if (!numButton->isSwitched()) {
        ui->alphaWidget->hide();

        numVerticalLayout->addWidget(label);
        numVerticalLayout->addWidget(display);
        ui->numGrid->addWidget(enterButton, 5, 16, 1, 4);
        ui->numGrid->addWidget(spaceButton, 5, 4, 1, 12);
        ui->numGrid->addWidget(delButton, 4, 15, 1, 5);
        ui->numGrid->addWidget(numButton, 5, 0, 1, 4);

        ui->numWidget->show();
    }
    else {
        if (symButton->isSwitched()) symPressed();
        ui->numWidget->hide();

        alphaVerticalLayout->addWidget(label);
        alphaVerticalLayout->addWidget(display);
        ui->alphaGrid->addWidget(enterButton, 5, 16, 1, 4);
        ui->alphaGrid->addWidget(spaceButton, 5, 4, 1, 12);
        ui->alphaGrid->addWidget(delButton, 4, 17, 1, 3);
        ui->alphaGrid->addWidget(numButton, 5, 0, 1, 4);

        ui->alphaWidget->show();
    }
    numButton->switchLabel();
}

void KeyboardDialog::symPressed() {
    for (int i = 0; i < 25; ++i)
        numButtons[i]->switchLabel();
    symButton->switchLabel();
}

KeyButton *KeyboardDialog::makeButton(const char *slot,
                                      const QString &firstLabel,
                                      const QString &secondLabel) {
    KeyButton *button = new KeyButton(firstLabel, secondLabel, this);
    connect(button, SIGNAL(clicked()), this, slot);
    return button;
}

void KeyboardDialog::setupAlphaButtons() {
    alphaButtons[0] = makeButton(SLOT(symbolPressed()), "q", "Q");
    alphaButtons[1] = makeButton(SLOT(symbolPressed()), "w", "W");
    alphaButtons[2] = makeButton(SLOT(symbolPressed()), "e", "E");
    alphaButtons[3] = makeButton(SLOT(symbolPressed()), "r", "R");
    alphaButtons[4] = makeButton(SLOT(symbolPressed()), "t", "T");
    alphaButtons[5] = makeButton(SLOT(symbolPressed()), "y", "Y");
    alphaButtons[6] = makeButton(SLOT(symbolPressed()), "u", "U");
    alphaButtons[7] = makeButton(SLOT(symbolPressed()), "i", "I");
    alphaButtons[8] = makeButton(SLOT(symbolPressed()), "o", "O");
    alphaButtons[9] = makeButton(SLOT(symbolPressed()), "p", "P");

    alphaButtons[10] = makeButton(SLOT(symbolPressed()), "a", "A");
    alphaButtons[11] = makeButton(SLOT(symbolPressed()), "s", "S");
    alphaButtons[12] = makeButton(SLOT(symbolPressed()), "d", "D");
    alphaButtons[13] = makeButton(SLOT(symbolPressed()), "f", "F");
    alphaButtons[14] = makeButton(SLOT(symbolPressed()), "g", "G");
    alphaButtons[15] = makeButton(SLOT(symbolPressed()), "h", "H");
    alphaButtons[16] = makeButton(SLOT(symbolPressed()), "j", "J");
    alphaButtons[17] = makeButton(SLOT(symbolPressed()), "k", "K");
    alphaButtons[18] = makeButton(SLOT(symbolPressed()), "l", "L");

    alphaButtons[19] = makeButton(SLOT(symbolPressed()), "z", "Z");
    alphaButtons[20] = makeButton(SLOT(symbolPressed()), "x", "X");
    alphaButtons[21] = makeButton(SLOT(symbolPressed()), "c", "C");
    alphaButtons[22] = makeButton(SLOT(symbolPressed()), "v", "V");
    alphaButtons[23] = makeButton(SLOT(symbolPressed()), "b", "B");
    alphaButtons[24] = makeButton(SLOT(symbolPressed()), "n", "N");
    alphaButtons[25] = makeButton(SLOT(symbolPressed()), "m", "M");

    for (int i = 0; i < 10; ++i)
        ui->alphaGrid->addWidget(alphaButtons[i], 2, i * 2, 1, 2);

    for (int i = 0; i < 9; ++i)
        ui->alphaGrid->addWidget(alphaButtons[i + 10], 3, i * 2 + 1, 1, 2);

    for (int i = 0; i < 7; ++i)
        ui->alphaGrid->addWidget(alphaButtons[i + 19], 4, i * 2 + 3, 1, 2);
}

void KeyboardDialog::setupNumButtons() {
    numButtons[0] = makeButton(SLOT(symbolPressed()), "1", "[");
    numButtons[1] = makeButton(SLOT(symbolPressed()), "2", "]");
    numButtons[2] = makeButton(SLOT(symbolPressed()), "3", "{");
    numButtons[3] = makeButton(SLOT(symbolPressed()), "4", "}");
    numButtons[4] = makeButton(SLOT(symbolPressed()), "5", "#");
    numButtons[5] = makeButton(SLOT(symbolPressed()), "6", "%");
    numButtons[6] = makeButton(SLOT(symbolPressed()), "7", "^");
    numButtons[7] = makeButton(SLOT(symbolPressed()), "8", "*");
    numButtons[8] = makeButton(SLOT(symbolPressed()), "9", "+");
    numButtons[9] = makeButton(SLOT(symbolPressed()), "0", "=");

    numButtons[10] = makeButton(SLOT(symbolPressed()), "-", "_");
    numButtons[11] = makeButton(SLOT(symbolPressed()), "/", "\\");
    numButtons[12] = makeButton(SLOT(symbolPressed()), ":", "|");
    numButtons[13] = makeButton(SLOT(symbolPressed()), ";", "~");
    numButtons[14] = makeButton(SLOT(symbolPressed()), "(", "<");
    numButtons[15] = makeButton(SLOT(symbolPressed()), ")", ">");
    numButtons[16] = makeButton(SLOT(symbolPressed()), "$", QChar(0x20AC));
    numButtons[17] = makeButton(SLOT(symbolPressed()), "&", QChar(0x00A3));
    numButtons[18] = makeButton(SLOT(symbolPressed()), "@", QChar(0x00A5));
    numButtons[19] = makeButton(SLOT(symbolPressed()), "\"", QChar(0x2022));

    numButtons[20] = makeButton(SLOT(symbolPressed()), ".", ".");
    numButtons[21] = makeButton(SLOT(symbolPressed()), ",", ",");
    numButtons[22] = makeButton(SLOT(symbolPressed()), "?", "?");
    numButtons[23] = makeButton(SLOT(symbolPressed()), "!", "!");
    numButtons[24] = makeButton(SLOT(symbolPressed()), "'", "'");

    for (int i = 0; i < 10; i++) {
        ui->numGrid->addWidget(numButtons[i], 2, i * 2, 1, 2);
        ui->numGrid->addWidget(numButtons[i + 10], 3, i * 2, 1, 2);
    }

    for (int i = 0; i < 5; i++)
        ui->numGrid->addWidget(numButtons[i + 20], 4, i * 2 + 5, 1, 2);
}

void KeyboardDialog::setupSpecialButtons() {
    enterButton = makeButton(SLOT(accept()), "Ent");
    spaceButton = makeButton(SLOT(spacePressed()), "Space");
    shiftButton = makeButton(SLOT(shiftPressed()), "Sft");
    delButton = makeButton(SLOT(delPressed()), "Del");
    numButton = makeButton(SLOT(numPressed()), "123", "ABC");
    symButton = makeButton(SLOT(symPressed()), "#+=", "123");

    ui->alphaGrid->addWidget(enterButton, 5, 16, 1, 4);
    ui->alphaGrid->addWidget(spaceButton, 5, 4, 1, 12);
    ui->alphaGrid->addWidget(shiftButton, 4, 0, 1, 3);
    ui->alphaGrid->addWidget(delButton, 4, 17, 1, 3);
    ui->alphaGrid->addWidget(numButton, 5, 0, 1, 4);
    ui->numGrid->addWidget(symButton, 4, 0, 1, 5);
}
