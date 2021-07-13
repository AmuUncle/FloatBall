#ifndef FLOATBALL_H
#define FLOATBALL_H

#include <QWidget>

class FloatBall : public QWidget
{
    Q_OBJECT

public:
    FloatBall(QWidget *parent = 0);
    ~FloatBall();

private:
    void CreateAllChildWnd();
    void InitCtrl();
    void InitSolts();
    void Relayout();


    void paintEvent(QPaintEvent *event);
    void timerEvent(QTimerEvent *event);
    void enterEvent(QEvent *event);
    void leaveEvent(QEvent *event);
    void mouseDoubleClickEvent(QMouseEvent * event);

private slots:
    void OnUpdateMemInfo();

private:
    uint m_dwMemoryLoad;
    int m_nUpdateMemTimerId;
    int m_nLeaveTimerId;
    int m_nCleanTimerId;
    bool m_bIsMini;
    bool m_bIsCleaning;
    int m_nDirection;
};

#endif // FLOATBALL_H
