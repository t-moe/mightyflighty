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

    enum class Buttons {
        Button1 = 49,
        Button2 = 112,
        Button3 = 51,
        Button4 = 7,
    };
    Q_ENUM(Buttons)

    enum class Leds {
        Led1 = 61,
        Led2 = 44,
        Led3 = 68,
        Led4 = 67,
    };
    Q_ENUM(Leds)

    Q_INVOKABLE const QVector<Buttons>& allButtons() const;
    Q_INVOKABLE const QVector<Leds>& allLeds() const;


    Q_INVOKABLE bool isActive(Buttons button) const;
    Q_INVOKABLE bool isActive(Leds led) const;
    Q_INVOKABLE void setState(Leds led, bool active);

 private:
    int _timerId;
    QVector<bool> _lastButtonStates;
 protected:
    void timerEvent(QTimerEvent* ev) Q_DECL_OVERRIDE;

signals:
    void buttonPressed(Buttons button);
    void buttonStateChanged(Buttons button, bool isActive);
};

#endif // IOCONTROLLER_H
