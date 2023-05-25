#include "NumpadDialog.h"
#include "LineEdit.h"
#include "ui_NumpadDialog.h"

NumpadDialog::NumpadDialog(const QString &text,
                           InputType type,
                           const double &min,
                           const double &max,
                           QWidget *parent)
    : InputProviderDialog(parent),
      ui(new Ui::NumpadDialog),
      m_decimalExists(false),
      m_isEmpty(true),
      m_min(min),
      m_max(max) {
    ui->setupUi(this);
    grid = qobject_cast<QGridLayout *>(layout());
    verticalLayout = new QVBoxLayout();
    verticalLayout->setContentsMargins(0, 3, 0, 9);

    QFont font;
    font.setPointSize(18);

    /* Setup the title label */
    if (m_min != -std::numeric_limits<double>::max() &&
        m_max != std::numeric_limits<double>::max())
        label = new QLabel(text + " (" + QString::number(m_min) + tr(" to ") +
                           QString::number(m_max) + ")");
    else
        label = new QLabel(text);
    label->setFont(font);
    verticalLayout->addWidget(label);

    /* Setup the display */
    display = new QLineEdit(this);
    display->setFocusPolicy(Qt::NoFocus);
    display->setAlignment(Qt::AlignRight);
    display->setFont(font);
    verticalLayout->addWidget(display);

    grid->addLayout(verticalLayout, 0, 0, 2, -1);

    /* Setup and add digit buttons */
    KeyButton *numberButtons[10];
    for (int i = 0; i < 10; ++i) {
        numberButtons[i] =
            makeButton(SLOT(numberPressed()), QString::number(i));
        if (!i) continue;
        const int row = (9 - i) / 3 + 2;
        const int column = (i - 1) % 3;
        grid->addWidget(numberButtons[i], row, column);
    }
    grid->addWidget(numberButtons[0], 5, 1);

    /* Setup and add other buttons */
    KeyButton *enterButton = makeButton(SLOT(accept()), tr("ENT"));
    KeyButton *delButton = makeButton(SLOT(delPressed()), tr("DEL"));
    KeyButton *clearButton = makeButton(SLOT(clearPressed()), tr("C"), "X");
    clearButton->switchLabel();
    KeyButton *decimalButton = makeButton(SLOT(decimalPressed()), ".");
    KeyButton *signButton = makeButton(SLOT(signPressed()), "+/-");
    grid->addWidget(enterButton, 4, 3, 2, 1);
    grid->addWidget(delButton, 3, 3);
    grid->addWidget(clearButton, 2, 3);
    grid->addWidget(decimalButton, 5, 2);
    grid->addWidget(signButton, 5, 0);

    /* Hide some buttons (if needed) */
    if (type != NumpadDialog::Decimal) decimalButton->hide();
    if (m_min >= 0.0) signButton->hide();

    connect(display,
            SIGNAL(textChanged(QString)),
            this,
            SLOT(displayChanged(QString)));
}

NumpadDialog::~NumpadDialog() { delete ui; }

void NumpadDialog::setInput(const QString &input) { display->setText(input); }

QString NumpadDialog::input() const { return display->text(); }

void NumpadDialog::displayChanged(QString text) {
    if (m_isEmpty) {
        m_isEmpty = false;
        KeyButton *button =
            qobject_cast<KeyButton *>(grid->itemAtPosition(2, 3)->widget());
        button->switchLabel();
    }
    else if (text.isEmpty()) {
        m_isEmpty = true;
        KeyButton *button =
            qobject_cast<KeyButton *>(grid->itemAtPosition(2, 3)->widget());
        button->switchLabel();
    }
}

void NumpadDialog::numberPressed() {
    KeyButton *button = qobject_cast<KeyButton *>(sender());
    QString newText = display->text() + button->text();
    if (inBounds(newText.toDouble())) display->setText(newText);
}

void NumpadDialog::delPressed() {
    QString text = display->text();
    if (text.endsWith(".")) m_decimalExists = false;
    text.chop(1);
    if (inBounds(text.toDouble())) display->setText(text);
}

void NumpadDialog::clearPressed() {
    KeyButton *button = qobject_cast<KeyButton *>(sender());
    if (!button->isSwitched()) {
        display->clear();
        m_decimalExists = false;
    }
    else
        reject();
}

void NumpadDialog::decimalPressed() {
    if (m_decimalExists) return;
    m_decimalExists = true;
    display->setText(display->text() + ".");
}

void NumpadDialog::signPressed() {
    QString text = display->text();
    if (text.startsWith("-"))
        text = text.right(text.length() - 1);
    else
        text = "-" + text;

    if (inBounds(text.toDouble())) display->setText(text);
}

KeyButton *NumpadDialog::makeButton(const char *slot,
                                    const QString &firstLabel,
                                    const QString &secondLabel) {
    KeyButton *button = new KeyButton(firstLabel, secondLabel, this);
    connect(button, SIGNAL(clicked()), this, slot);
    return button;
}

bool NumpadDialog::inBounds(const double &value) const {
    return (value >= m_min && value <= m_max);
}
