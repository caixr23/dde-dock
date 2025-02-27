// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef SPLITERANIMATED_H
#define SPLITERANIMATED_H

#include <constants.h>

#include <QWidget>
#include <QVariantAnimation>

class SpliterAnimated : public QWidget
{
    Q_OBJECT
public:
    explicit SpliterAnimated(QWidget *parent = nullptr);

    void setStartValue(const QVariant &value);
    void setEndValue(const QVariant &value);
//    void startAnimation();
//    void stopAnimation();
    void setDockPosition(const Dock::Position position);

protected:
    void paintEvent(QPaintEvent *event) override;

private Q_SLOTS:
    void onSizeAnimationValueChanged(const QVariant &value);

private:
    QVariantAnimation *m_sizeAnimation;

    qreal m_opacityChangeStep;
    qreal m_currentOpacity;

    Dock::Position m_dockPosition;
};

#endif // SPLITERANIMATED_H
