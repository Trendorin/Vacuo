#include "scan_model.hpp"

#include "vacuo/core/types.hpp"

#include <QBrush>
#include <QColor>
#include <QString>

#include <algorithm>

ScanModel::ScanModel(QObject* parent) : QAbstractTableModel(parent) {}

int ScanModel::rowCount(const QModelIndex& parent) const {
    return parent.isValid() ? 0 : static_cast<int>(report_.results.size());
}

int ScanModel::columnCount(const QModelIndex& parent) const {
    return parent.isValid() ? 0 : ColumnCount;
}

QVariant ScanModel::data(const QModelIndex& index, const int role) const {
    if (!index.isValid() || index.row() < 0
        || static_cast<std::size_t>(index.row()) >= report_.results.size()) {
        return {};
    }
    const auto& result = report_.results[static_cast<std::size_t>(index.row())];

    if (role == Qt::CheckStateRole && index.column() == Selected) {
        return static_cast<int>(selected_[static_cast<std::size_t>(index.row())]
                                    ? Qt::Checked
                                    : Qt::Unchecked);
    }
    if (role == Qt::TextAlignmentRole && (index.column() == Size || index.column() == Items)) {
        return static_cast<int>(Qt::AlignRight | Qt::AlignVCenter);
    }
    if (role == Qt::ForegroundRole && !result.available) {
        return QBrush(QColor(Qt::gray));
    }
    if (role == Qt::ToolTipRole) {
        QString tooltip = QString::fromStdString(result.rule.description);
        if (!result.rule.warning.empty()) {
            tooltip += QStringLiteral("\n\n") + QString::fromStdString(result.rule.warning);
        }
        if (!result.note.empty()) {
            tooltip += QStringLiteral("\n\n") + QString::fromStdString(result.note);
        }
        return tooltip;
    }
    if (role != Qt::DisplayRole) {
        return {};
    }

    switch (index.column()) {
    case Selected:
        return {};
    case Category:
        return QString::fromStdString(result.rule.title);
    case Scope:
        return result.rule.scope == vacuo::RuleScope::User ? tr("User") : tr("System");
    case Risk:
        switch (result.rule.risk) {
        case vacuo::RiskLevel::Low:
            return tr("Low");
        case vacuo::RiskLevel::Moderate:
            return tr("Moderate");
        case vacuo::RiskLevel::Elevated:
            return tr("Elevated");
        }
        return tr("Unknown");
    case Size:
        return QString::fromStdString(vacuo::formatBytes(result.stats.bytes));
    case Items:
        return QString::number(result.stats.items);
    case Status:
        if (!result.available) {
            return tr("Not present");
        }
        if (result.stats.truncated) {
            return tr("Partial scan");
        }
        if (result.stats.inaccessible > 0) {
            return tr("Limited access");
        }
        return result.stats.items == 0 ? tr("Clean") : tr("Ready");
    default:
        return {};
    }
}

QVariant ScanModel::headerData(const int section,
                               const Qt::Orientation orientation,
                               const int role) const {
    if (orientation != Qt::Horizontal || role != Qt::DisplayRole) {
        return QAbstractTableModel::headerData(section, orientation, role);
    }
    switch (section) {
    case Selected:
        return tr("Clean");
    case Category:
        return tr("Category");
    case Scope:
        return tr("Scope");
    case Risk:
        return tr("Risk");
    case Size:
        return tr("Size");
    case Items:
        return tr("Items");
    case Status:
        return tr("Status");
    default:
        return {};
    }
}

Qt::ItemFlags ScanModel::flags(const QModelIndex& index) const {
    auto itemFlags = QAbstractTableModel::flags(index);
    if (!index.isValid()) {
        return itemFlags;
    }
    const auto& result = report_.results[static_cast<std::size_t>(index.row())];
    if (!result.available || result.stats.items == 0) {
        return itemFlags & ~Qt::ItemIsEnabled;
    }
    if (index.column() == Selected) {
        itemFlags |= Qt::ItemIsUserCheckable;
    }
    return itemFlags;
}

bool ScanModel::setData(const QModelIndex& index, const QVariant& value, const int role) {
    if (!index.isValid() || index.column() != Selected || role != Qt::CheckStateRole
        || static_cast<std::size_t>(index.row()) >= selected_.size()) {
        return false;
    }
    const auto& result = report_.results[static_cast<std::size_t>(index.row())];
    if (!result.available || result.stats.items == 0) {
        return false;
    }
    selected_[static_cast<std::size_t>(index.row())] = value.toInt() == Qt::Checked;
    emit dataChanged(index, index, {Qt::CheckStateRole});
    return true;
}

void ScanModel::setReport(vacuo::ScanReport report) {
    beginResetModel();
    report_ = std::move(report);
    selected_.assign(report_.results.size(), false);
    for (std::size_t index = 0; index < report_.results.size(); ++index) {
        const auto& result = report_.results[index];
        selected_[index] = result.available && result.stats.items > 0 && result.rule.selectedByDefault;
    }
    endResetModel();
}

const vacuo::ScanReport& ScanModel::report() const {
    return report_;
}

const vacuo::ScanResult* ScanModel::resultAt(const int row) const {
    if (row < 0 || static_cast<std::size_t>(row) >= report_.results.size()) {
        return nullptr;
    }
    return &report_.results[static_cast<std::size_t>(row)];
}

std::vector<std::string> ScanModel::selectedRuleIds() const {
    std::vector<std::string> ids;
    for (std::size_t index = 0; index < selected_.size(); ++index) {
        if (selected_[index]) {
            ids.push_back(report_.results[index].rule.id);
        }
    }
    return ids;
}

std::uint64_t ScanModel::selectedBytes() const {
    std::uint64_t total = 0;
    for (std::size_t index = 0; index < selected_.size(); ++index) {
        if (selected_[index]) {
            total += report_.results[index].stats.bytes;
        }
    }
    return total;
}
