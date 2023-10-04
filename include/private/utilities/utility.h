#pragma once

#include <QFont>
#include <QPoint>
#include <QPainterPath>
#include <optional>

#include "type/point.pb.h"
#include "type/color.pb.h"

#include "qtgraph.h"

namespace qtgraph {

class BaseNode;
enum class EPinDirection;
class TypedNode;

std::optional<QJsonObject> loadFile(const char* name);

QFont standardFont(int size);

QPoint snap(const QPointF &position, short interval);

QPainterPath standardPath(const QPoint &origin, const QPoint &target, float zoomMult = 1.0f);

protocol::type::Color   convertTo_protocolColor(const QColor &color);
QColor convertFrom_protocolColor(const protocol::type::Color &color);

protocol::type::Point   convertTo_protocolPoint(const QPoint &point);
QPoint convertFrom_protocolPoint(const protocol::type::Point &point);
protocol::type::PointF  convertTo_protocolPointF(const QPointF &point);
QPointF convertFrom_protocolPointF(const protocol::type::PointF &point);

// Parses string like "A2C2AA" into QColor
QColor parseToColor(const QString &str);

}
