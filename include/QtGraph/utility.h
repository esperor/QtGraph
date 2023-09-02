#pragma once

#include <QFont>
#include <QPoint>
#include <QPainterPath>
#include <optional>

#include "type/point.pb.h"
#include "type/color.pb.h"

#include "QtGraph/GraphLib.h"

namespace GraphLib {

class BaseNode;
enum class PinDirection;
class TypedNode;

std::optional<QJsonObject> loadFile(const char* name);

QFont GRAPHLIB_EXPORT standardFont(int size);

QPoint GRAPHLIB_EXPORT snap(const QPointF &position, short interval);

QPainterPath GRAPHLIB_EXPORT standardPath(const QPoint &origin, const QPoint &target, float zoomMult = 1.0f);

protocol::type::Color   convertTo_protocolColor(const QColor &color);

protocol::type::Point   convertTo_protocolPoint(const QPoint &point);
protocol::type::PointF  convertTo_protocolPointF(const QPointF &point);

namespace NodeFactoryModule {

QColor GRAPHLIB_EXPORT parseToColor(const QString &str);

}

}
