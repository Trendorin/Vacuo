#include "settings_dialog.hpp"

#include <QComboBox>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QGroupBox>
#include <QLabel>
#include <QPalette>
#include <QPushButton>
#include <QVBoxLayout>

SettingsDialog::SettingsDialog(const QString& language, QWidget* parent) : QDialog(parent) {
    setWindowTitle(tr("Settings"));
    setModal(true);
    setMinimumWidth(420);

    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(18, 18, 18, 18);
    root->setSpacing(12);

    auto* interfaceGroup = new QGroupBox(tr("Interface"), this);
    auto* form = new QFormLayout(interfaceGroup);
    language_ = new QComboBox(interfaceGroup);
    language_->addItem(tr("System language"), QStringLiteral("system"));
    language_->addItem(QStringLiteral("English"), QStringLiteral("en"));
    language_->addItem(QStringLiteral("Русский"), QStringLiteral("ru"));
    language_->addItem(QStringLiteral("Українська"), QStringLiteral("uk"));
    language_->addItem(QStringLiteral("Deutsch"), QStringLiteral("de"));
    const int languageIndex = language_->findData(language);
    language_->setCurrentIndex(languageIndex >= 0 ? languageIndex : 0);
    form->addRow(tr("Language:"), language_);
    root->addWidget(interfaceGroup);

    auto* hint = new QLabel(tr("The interface language changes immediately after saving."), this);
    hint->setWordWrap(true);
    hint->setForegroundRole(QPalette::PlaceholderText);
    root->addWidget(hint);

    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Cancel | QDialogButtonBox::Save, this);
    buttons->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));
    buttons->button(QDialogButtonBox::Save)->setText(tr("Save"));
    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    root->addWidget(buttons);
}

QString SettingsDialog::language() const {
    return language_->currentData().toString();
}
