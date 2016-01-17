#include "iocontroller.h"
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <QHash>

//--- Lowlevel helper code ahead. TODO: Cleanup and refactor it ---
#ifdef IS_EMBEDDED

#define SYSFS_PATH		"/sys/class/gpio/"
#define MAX_STR_BUF		512
#define MAX_PATH_STR   (512)
#define LOW             0
#define HIGH			1
#define OUTPUT			0
#define INPUT			1


enum {
  GPIO_SYSFS_EXPORT = 0,
  GPIO_SYSFS_UNEXPORT,
  GPIO_SYSFS_GET_DIRECTION,
  GPIO_SYSFS_SET_DIRECTION,
  GPIO_SYSFS_GET_VALUE,
  GPIO_SYSFS_SET_VALUE,
 };

/*
 ***************************************************************************
 * sysfs_gpio_handler - handle GPIO operations
 * function: export, unexport, set direction, set and get values
 * gpio:     pin number of the gpio [0..127]
 * value:    value to write or null
 ***************************************************************************
 */
static int sysfs_gpio_handler(uint8_t function, uint32_t gpio, const char *val)
 {
  char     path_str[MAX_PATH_STR];
  char     strBuf[MAX_STR_BUF];
  uint8_t  oflags = 0;
  int32_t  fd;
  uint32_t len;
  uint8_t  inval;

  /* Determine open flags based on function */
  switch (function)
   {
    case GPIO_SYSFS_EXPORT:
     snprintf(path_str, sizeof(path_str), SYSFS_PATH"export");
     oflags=O_WRONLY;
    break;

    case GPIO_SYSFS_UNEXPORT:
     snprintf(path_str, sizeof(path_str), SYSFS_PATH"unexport");
     oflags=O_WRONLY;
    break;

    case GPIO_SYSFS_SET_DIRECTION:
     snprintf(path_str, sizeof(path_str), SYSFS_PATH"gpio%d/direction", gpio);
     oflags=O_WRONLY;
    break;

    case GPIO_SYSFS_SET_VALUE:
     snprintf(path_str, sizeof(path_str), SYSFS_PATH"gpio%d/value", gpio);
     oflags=O_WRONLY;
    break;

    case GPIO_SYSFS_GET_DIRECTION:
    case GPIO_SYSFS_GET_VALUE:
     snprintf(path_str, sizeof(path_str), SYSFS_PATH"gpio%d/value", gpio);
     oflags=O_RDONLY;
    break;

    default:
     printf("File operation flag not defined\n");
   }

  /* Open the pseudo file given its path and open flags	*/
  #ifdef DEBUG
   printf("open:%s\n", path_str);
  #endif

  fd = open(path_str, oflags);
  if (fd  < 0)
   {
    perror(path_str);
    return fd;
   }

  /* File operations r/w on the opened file */
  switch (function)
   {
    case GPIO_SYSFS_EXPORT:
    case GPIO_SYSFS_UNEXPORT:
     len = snprintf(strBuf, sizeof(strBuf), "%d", gpio);
     #ifdef DEBUG
      printf("exp/unexp:%s\n", strBuf);
     #endif
     write(fd, strBuf, len);
    break;

    case GPIO_SYSFS_SET_DIRECTION:
     #ifdef DEBUG
       printf("write dir:%s\n", val);
     #endif
     write(fd, val, strlen(val)+1);
    break;

    case GPIO_SYSFS_SET_VALUE:
     #ifdef DEBUG
      printf("write val:%s\n", val);
     #endif
     write(fd, val, strlen(val)+1);
    break;

    case GPIO_SYSFS_GET_DIRECTION:
    break;

    case GPIO_SYSFS_GET_VALUE:
     read(fd, &inval, 1);
     #ifdef DEBUG
      printf("read val:%c\n", inval);
     #endif
     close(fd);
     return inval=='0';
    break;

    default:
     printf("function not defined\n");
   }
  close(fd);
  return 0;
 }

static float sysfs_get_adc() {
    int32_t fd = open("/sys/bus/iio/devices/iio:device0/in_voltage4_raw", O_RDONLY);
    static char buf[16];
    if (fd  < 0)
     {
      perror("cannot access adc dev");
      return fd;
     }
    int charRead = read(fd, buf, sizeof(buf));
    close(fd);
    if (charRead != -1)
     {
      buf[charRead] = '\0';

      float aValue = (float) atoi(buf);
      aValue = (1.8 * aValue) / ((1<<12)-1);

      return aValue;
    }
    return -1;
}


static QHash<int,int> buttonGpio = {{IoController::Buttons::Button1,49},
                                        {IoController::Buttons::Button2,112},
                                        {IoController::Buttons::Button3,51},
                                        {IoController::Buttons::Button4,7}};

static QHash<int,int> ledGpio = {{IoController::Leds::Led1,61},
                                  {IoController::Leds::Led2,44},
                                  {IoController::Leds::Led3,68},
                                  {IoController::Leds::Led4,67}};

#endif

//--- Class implementation ahead ---

IoController::IoController()
{
    #ifdef IS_EMBEDDED
    for(int i=0; i<buttonCount(); i++) {
        sysfs_gpio_handler(GPIO_SYSFS_EXPORT, buttonGpio[i], NULL);
        sysfs_gpio_handler(GPIO_SYSFS_SET_DIRECTION, buttonGpio[i], "in");
    }

    for(int i=0; i<ledCount(); i++) {
        sysfs_gpio_handler(GPIO_SYSFS_EXPORT, ledGpio[i], NULL);
        sysfs_gpio_handler(GPIO_SYSFS_SET_DIRECTION, ledGpio[i],"out");
        sysfs_gpio_handler(GPIO_SYSFS_SET_VALUE, ledGpio[i], "1");
    }
    #endif

     for(int i=0; i<buttonCount(); i++) {
         _lastButtonStates.append(isActive(static_cast<Buttons>(i)));
         _buttonPressTime.append(0);
     }
     _lastPotiValue = potiValue();

    _timerId = startTimer(50);
}

IoController::~IoController()
{
    #ifdef IS_EMBEDDED
    killTimer(_timerId);
    for(int i=0; i<buttonCount(); i++) {
        sysfs_gpio_handler(GPIO_SYSFS_UNEXPORT, buttonGpio[i], NULL);
    }
    for(int i=0; i<ledCount(); i++) {
        sysfs_gpio_handler(GPIO_SYSFS_UNEXPORT, ledGpio[i], NULL);
    }
#endif
}

int IoController::buttonCount() const
{
    #ifdef IS_EMBEDDED
    return buttonGpio.count();
    #else
    return 0;
    #endif
}

int IoController::ledCount() const
{
     #ifdef IS_EMBEDDED
    return ledGpio.count();
    #else
    return 0;
    #endif
}


bool IoController::isActive(IoController::Buttons button) const
{
    #ifdef IS_EMBEDDED
    return sysfs_gpio_handler(GPIO_SYSFS_GET_VALUE, buttonGpio[button], NULL);
    #else
    return false;
    #endif
}

bool IoController::isActive(IoController::Leds led) const
{
    #ifdef IS_EMBEDDED
    return sysfs_gpio_handler(GPIO_SYSFS_GET_VALUE, ledGpio[led], NULL);
    #else
    return false;
    #endif
}

void IoController::setState(Leds led, bool active)
{
    #ifdef IS_EMBEDDED
    sysfs_gpio_handler(GPIO_SYSFS_SET_VALUE, ledGpio[led], active ? "0":"1");
    #endif
}

float IoController::potiValue() const
{
    #ifdef IS_EMBEDDED
    return sysfs_get_adc();
    #else
    return 0;
    #endif
}


void IoController::timerEvent(QTimerEvent*)
{
    for(int i=0; i<buttonCount(); i++) {
        Buttons b = static_cast<Buttons>(i);
        bool curState = isActive(b);
        bool lastState = _lastButtonStates[i];
        _lastButtonStates[i] = curState;
        if(curState!=lastState) {
            emit buttonStateChanged(b,curState);
        }
        if(!curState && lastState && _buttonPressTime[i] < 20) {
            emit buttonPressed(b);
        } else if(_buttonPressTime[i]==20) {
            emit buttonLongPressed(b);
        }
        if(curState) {
            _buttonPressTime[i]++;
        } else {
            _buttonPressTime[i]=0;
        }
    }
    float curPotiValue = potiValue();
    #define MIN_CHANGE 0.015
    if(fabs(curPotiValue-_lastPotiValue) > MIN_CHANGE) {
        _lastPotiValue = curPotiValue;
        emit potiValueChanged(curPotiValue);
    }

}
