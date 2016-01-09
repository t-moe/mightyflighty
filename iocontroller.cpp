#include "iocontroller.h"
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

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
int sysfs_gpio_handler(uint8_t function, uint32_t gpio, const char *val)
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

#endif

//--- Class implementation ahead ---

IoController::IoController()
{
    #ifdef IS_EMBEDDED
    for(int i=0; i<allButtons().count(); i++) {
        sysfs_gpio_handler(GPIO_SYSFS_EXPORT, static_cast<uint32_t>(allButtons()[i]), NULL);
        sysfs_gpio_handler(GPIO_SYSFS_SET_DIRECTION, static_cast<uint32_t>(allButtons()[i]), "in");
    }

    for(int i=0; i<allLeds().count(); i++) {
        sysfs_gpio_handler(GPIO_SYSFS_EXPORT, static_cast<uint32_t>(allLeds()[i]), NULL);
        sysfs_gpio_handler(GPIO_SYSFS_SET_DIRECTION, static_cast<uint32_t>(allLeds()[i]), "out");
        sysfs_gpio_handler(GPIO_SYSFS_SET_VALUE, static_cast<uint32_t>(allLeds()[i]), "1");
    }
    #endif

     for(int i=0; i<allButtons().count(); i++) {
         _lastButtonStates.append(isActive(allButtons()[i]));
     }
    _timerId = startTimer(50);
}

IoController::~IoController()
{
    #ifdef IS_EMBEDDED
    killTimer(_timerId);
    for(int i=0; i<allButtons().count(); i++) {
        sysfs_gpio_handler(GPIO_SYSFS_UNEXPORT, static_cast<uint32_t>(allButtons()[i]), NULL);
    }
    for(int i=0; i<allLeds().count(); i++) {
        sysfs_gpio_handler(GPIO_SYSFS_UNEXPORT, static_cast<uint32_t>(allLeds()[i]), NULL);
    }
    #endif
}

const QVector<IoController::Buttons> &IoController::allButtons() const
{
    static QVector<Buttons> buttons = {Buttons::Button1, Buttons::Button2, Buttons::Button3, Buttons::Button4};
    return buttons;
}

const QVector<IoController::Leds> &IoController::allLeds() const
{
    static QVector<Leds> leds = {Leds::Led1, Leds::Led2, Leds::Led3, Leds::Led4};
    return leds;
}

bool IoController::isActive(IoController::Buttons button) const
{
    #ifdef IS_EMBEDDED
    return sysfs_gpio_handler(GPIO_SYSFS_GET_VALUE, static_cast<uint32_t>(button), NULL);
    #else
    return false;
    #endif
}

bool IoController::isActive(IoController::Leds led) const
{
    #ifdef IS_EMBEDDED
    return sysfs_gpio_handler(GPIO_SYSFS_GET_VALUE, static_cast<uint32_t>(led), NULL);
    #else
    return false;
    #endif
}

void IoController::setState(Leds led, bool active)
{
    #ifdef IS_EMBEDDED
    sysfs_gpio_handler(GPIO_SYSFS_SET_VALUE, static_cast<uint32_t>(led), active ? "0":"1");
    #endif
}

void IoController::timerEvent(QTimerEvent*)
{
    for(int i=0; i<allButtons().count(); i++) {
        Buttons b = allButtons()[i];
        bool curState = isActive(b);
        bool lastState = _lastButtonStates[i];
        _lastButtonStates[i] = curState;
        if(curState!=lastState) {
            emit buttonStateChanged(b,curState);
        }
        if(curState && ! lastState) {
            emit buttonPressed(b);
        }
    }
}
