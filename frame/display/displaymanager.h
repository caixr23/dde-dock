// SPDX-FileCopyrightText: 2018 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef DISPLAYMANAGER_H
#define DISPLAYMANAGER_H

#include <QObject>

#include "singleton.h"
#include "constants.h"

using namespace Dock;

class QScreen;
class QTimer;
class QGSettings;
/**
 * @brief The DisplayManager class
 * @note    1、对QScreen信息的获取和监听进行了封装，不应该在此程序的其他处再出现类似的代码，而是从当前实例中进行提供
 * @note    2、对显示相关数据的读写尽可能通过Qt库进行，而不是通过一些DBus服务，
 * @note       目的一是为了解耦
 * @note       二是DBus服务的稳定性较低，有部分问题存在
 * @note       三是DBus服务(特指Display相关)刚开始的目的是为了做一些架构和特殊机器的兼容适配工作，因为qt适配开展较慢，所以才有了DBus服务的存在，目前qt已足够稳定，我们不应该再大量使用后端提供的Display服务
 */
class DisplayManager: public QObject, public Singleton<DisplayManager>
{
    Q_OBJECT
    friend class Singleton<DisplayManager>;

public:
    explicit DisplayManager(QObject *parent = Q_NULLPTR);

    QList<QScreen *> screens() const;
    QScreen *screen(const QString &screenName) const;
    QString primary() const;
    int screenRawWidth() const;
    int screenRawHeight() const;
    bool canDock(QScreen *s, Position pos) const;
    bool isCopyMode();

private:
    void updateScreenDockInfo();

private Q_SLOTS:
    void screenCountChanged();
    void dockInfoChanged();
    void onGSettingsChanged(const QString &key);

Q_SIGNALS:
    void primaryScreenChanged(QScreen *s);
    void screenInfoChanged();       // 屏幕信息发生变化，需要调整任务栏显示，只需要这一个信号，其他的都不要，简化流程
    void screenAdded(QScreen *s);
    void screenRemoved(QScreen *s);

private:
    QList<QScreen *> m_screens;
    QMap<QScreen *, QMap<Position, bool>> m_screenPositionMap;
    const QGSettings *m_gsettings;              // 多屏配置控制
    bool m_onlyInPrimary;
};

#endif // DISPLAYMANAGER_H
