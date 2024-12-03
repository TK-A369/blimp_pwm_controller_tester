#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <time.h>
#include <unistd.h>

#define CHANNELS 12

void send_pwm_duties(int i2c_fd, uint8_t* duties) {
  if (write(i2c_fd, duties, CHANNELS) != CHANNELS) {
    fprintf(stderr, "couldn't perform I2C write\n");
    exit(1);
  }
}

int main(int argc, char** argv) {
  int i2c_fd = open("/dev/i2c-1", O_RDWR);
  if (i2c_fd < 0) {
    fprintf(stderr, "couldn't open I2C\n");
    exit(1);
  }

  if (ioctl(i2c_fd, I2C_SLAVE, 0x37) < 0) {
    fprintf(stderr, "couldn't set I2C slave address\n");
    exit(1);
  }

  uint8_t duties_all_min[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  uint8_t duties_all_max[] = {255, 255, 255, 255, 255, 255,
                              255, 255, 255, 255, 255, 255};
  uint8_t duties_cross_1[] = {0, 255, 0, 255, 0, 255, 0, 255, 0, 255, 0, 255};
  uint8_t duties_incrementing[] = {0,   20,  40,  60,  80,  100,
                                   120, 140, 160, 180, 200, 220};

  uint8_t counter = 0;

  while (1) {
    /*uint8_t duties[CHANNELS];
    for (uint8_t i = 0; i < CHANNELS; i++) {
      duties[i] = (uint8_t)(rand() % 0x100);
    }*/

    uint8_t* curr_duties = duties_all_min;
    if (counter == 0)
      curr_duties = duties_all_min;
    else if (counter == 1)
      curr_duties = duties_all_max;
    else if (counter == 2)
      curr_duties = duties_cross_1;
    else if (counter == 3)
      curr_duties = duties_incrementing;

    send_pwm_duties(i2c_fd, curr_duties);
    printf("wrote PWM duties num %d\n", counter);

    counter++;
    counter %= 4;

    struct timespec sleep_time;
    sleep_time.tv_sec = 1;
    sleep_time.tv_nsec = 0;
    nanosleep(&sleep_time, NULL);
  }

  return 0;
}
