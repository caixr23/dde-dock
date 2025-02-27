// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "constants.h"
#include "abstracttraywidget.h"

#include <xcb/xproto.h>
#include <QMouseEvent>
#include <QDebug>

AbstractTrayWidget::AbstractTrayWidget(QWidget *parent, Qt::WindowFlags f)
    : QWidget(parent, f)
    , m_handleMouseReleaseTimer(new QTimer(this))
    , m_ownerPID(0)
{
    m_handleMouseReleaseTimer->setSingleShot(true);
    m_handleMouseReleaseTimer->setInterval(10);

    connect(m_handleMouseReleaseTimer, &QTimer::timeout, this, &AbstractTrayWidget::handleMouseRelease);
}

AbstractTrayWidget::~AbstractTrayWidget()
{

}

void AbstractTrayWidget::mousePressEvent(QMouseEvent *event)
{
    // call QWidget::mousePressEvent means to show dock-context-menu
    // when right button of mouse is pressed immediately in fashion mode

    // here we hide the right button press event when it is click in the special area
    if (event->button() == Qt::RightButton && perfectIconRect().contains(event->pos(), true)) {
        event->accept();
        return;
    }

    QWidget::mousePressEvent(event);
}

void AbstractTrayWidget::mouseReleaseEvent(QMouseEvent *e)
{
    //e->accept();

    // 由于 XWindowTrayWidget 中对 发送鼠标事件到X窗口的函数, 如 sendClick/sendHoverEvent 中
    // 使用了 setX11PassMouseEvent, 而每次调用 setX11PassMouseEvent 时都会导致产生 mousePress 和 mouseRelease 事件
    // 因此如果直接在这里处理事件会导致一些问题, 所以使用 Timer 来延迟处理 100 毫秒内的最后一个事件
    m_lastMouseReleaseData.first = e->pos();
    m_lastMouseReleaseData.second = e->button();

    m_handleMouseReleaseTimer->start();

    QWidget::mouseReleaseEvent(e);
}


void AbstractTrayWidget::handleMouseRelease()
{
    Q_ASSERT(sender() == m_handleMouseReleaseTimer);

    // do not dealwith all mouse event of SystemTray, class SystemTrayItem will handle it
    if (trayType() == SystemTray)
        return;

    const QPoint point(m_lastMouseReleaseData.first - rect().center());
    if (point.manhattanLength() > 24)
        return;

    QPoint globalPos = QCursor::pos();
    uint8_t buttonIndex = XCB_BUTTON_INDEX_1;

    switch (m_lastMouseReleaseData.second) {
    case Qt:: MiddleButton:
        buttonIndex = XCB_BUTTON_INDEX_2;
        break;
    case Qt::RightButton:
        buttonIndex = XCB_BUTTON_INDEX_3;
        break;
    default:
        break;
    }

    sendClick(buttonIndex, globalPos.x(), globalPos.y());

    // left mouse button clicked
    if (buttonIndex == XCB_BUTTON_INDEX_1) {
        Q_EMIT clicked();
    }
}

const QRect AbstractTrayWidget::perfectIconRect() const
{
    const QRect itemRect = rect();
    const int iconSize = std::min(itemRect.width(), itemRect.height());

    QRect iconRect;
    iconRect.setWidth(iconSize);
    iconRect.setHeight(iconSize);
    iconRect.moveTopLeft(itemRect.center() - iconRect.center());

    return iconRect;
}

void AbstractTrayWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);

    const Dock::Position position = qApp->property(PROP_POSITION).value<Dock::Position>();
    // 保持横纵比
    if (position == Dock::Bottom || position == Dock::Top) {
        setMaximumWidth(height());
        setMaximumHeight(QWIDGETSIZE_MAX);
    } else {
        setMaximumHeight(width());
        setMaximumWidth(QWIDGETSIZE_MAX);
    }
}

uint AbstractTrayWidget::getOwnerPID()
{
    return this->m_ownerPID;
}

void AbstractTrayWidget::setOwnerPID(uint PID)
{
    this->m_ownerPID = PID;
}
