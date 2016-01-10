#ifndef IOCONTROLLER_H
#define IOCONTROLLER_H

#include <QObject>
#include <QVector>

class IoController : public QObject
{
    Q_OBJECT
public:
    explicit IoController();
    ~IoController();

    enum Buttons {
        Button1 = 1,
        Button2 = 2,
        Button3 = 3,
        Button4 = 4,
    };
    Q_ENUM(Buttons)

    enum Leds {
        Led1 = 1,
        Led2 = 2,
        Led3 = 3,
        Led4 = 4,
    };
    Q_ENUM(Leds)

    Q_INVOKABLE int buttonCount() const;
    Q_INVOKABLE int ledCount() const;


    Q_INVOKABLE bool isActive(Buttons button) const;
    Q_INVOKABLE bool isActive(Leds led) const;
    Q_INVOKABLE void setState(Leds led, bool active);

 private:
    int _timerId;
    QVector<bool> _lastButtonStates;
    QVector<int> _buttonPressTime;
 protected:
    void timerEvent(QTimerEvent* ev) Q_DECL_OVERRIDE;

signals:
    void buttonPressed(Buttons button);
    void buttonLongPressed(Buttons button);
    void buttonStateChanged(Buttons button, bool isActive);
};

#endif // IOCONTROLLER_H
