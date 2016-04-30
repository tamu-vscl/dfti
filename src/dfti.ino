/*
 *  VSCL Developmental Flight Test Instrumentation Code
 *
 *  Reads in sensor values and stores to file.
 */

/* Arduino stdlib */
#include <SD.h>
#include <SPI.h>
/* Third-party libs */
#include <errno.h>
#include <Queue.h>
/* Custom libs */
#include <libsensor.h>
#include <libuadc.h>
#include <libvn200.h>


/* Configure serial devices. */
#define VN200_BAUD_RATE 115200
#define UADC_BAUD_RATE 115200
/* Configure SD card logging. */
#define SD_CARD_ETHSHD  4
#define SD_CARD_SPKFUN  8
#define FN_BUFLEN      50
/* Increment file to record the last log file number. */
#define INCREMENT_FILE "lastlog.txt"
/* Configure analog inputs. */
#define LEFT_AILERON_PIN      A0
#define RIGHT_AILERON_PIN     A1
#define LEFT_RUDDERVATOR_PIN  A2
#define RIGHT_RUDDERVATOR_PIN A3
#define LEFT_FLAP_PIN         A4
#define RIGHT_FLAP_PIN        A5
/* Configure PWM inputs. */
#define THROTTLE_INTERRUPT    0
/* Define rates for scheduler. */
#define TASK_SUCCESS     0
#define TASK_FAILURE     1
#define TASK_30SEC   30000
#define TASK_10SEC   10000
#define TASK_5SEC     5000
#define TASK_2SEC     2000
#define TASK_1HZ      1000
#define TASK_10HZ      100
#define TASK_100HZ      10
/* Debugging and failure states. */
#define FAULT_SDIO        0x01
#define FAULT_FILEIO      0x02
#define FAULT_SCHED_FLUSH 0x04
#define FAULT_SCHED_SAMPL 0x08
#define DEBUG


/* Global variables. */
/* File object for log. */
File fd;
/* Sensor objects. */
VN200 vn200;
UADC uadc;
/* Throttle PWM. */
volatile uint32_t throttle_pwm = 0;
volatile uint32_t throttle_last_us = 0;
/* Task scheduling queue. */
Queue queue;
/* Flag to ensure successful initialization. */
int8_t init_success = 0;
/* String separator. */
String sep = String(",");


void
setup()
{
    /* Setup h/w serial devices. */
    vn200 = VN200(&Serial2, VN200_BAUD_RATE);
    uadc = UADC(&Serial3, UADC_BAUD_RATE);
    vn200.begin();
    uadc.begin();

    /* Set up throttle PWM logging. */
    attachInterrupt(THROTTLE_INTERRUPT, throttle_pwm_rising, RISING);

    /*
     * Attempt to connect to the SD card. We first try to connect to the SD
     * card on an Ethernet shield, and if that fails then we attempt to connect
     * to the SD card on the Sparkfun SD shield.
     */
    if (!SD.begin(SD_CARD_ETHSHD)) {
        if (!SD.begin(SD_CARD_SPKFUN)) {
            init_success ^= FAULT_SDIO;
        }
    }
    /*
     * If we successfully we able to access an SD card on one of the shields,
     * we create the logfile.
     */
    if (!(init_success & FAULT_SDIO)) {
        if (open_logfile() < 0) {
            init_success ^= FAULT_FILEIO;
        }
    }

    /* Flush data to file every 10 seconds. */
    if (queue.scheduleFunction(flush_logfile, "FLUSH", 50, TASK_10SEC) < 0) {
        init_success ^= FAULT_SCHED_FLUSH;
    }
    /* Read in sensor data at 100 Hz and log. */
    if (queue.scheduleFunction(sample_sensors, "LOG", 100, TASK_100HZ) < 0) {
        init_success ^= FAULT_SCHED_SAMPL;
    }

#ifdef DEBUG
    /* Initialize debugging and alert to setup faults. */
    Serial.begin(9600);
    if (init_success & FAULT_SDIO) {
        Serial.println("FAULT: failed to access SD card.");
    }
    if (init_success & FAULT_FILEIO) {
        Serial.println("FAULT: failed to open SD card file(s).");
    }
    if (init_success & FAULT_SCHED_FLUSH) {
        Serial.println("FAULT: failed to schedule flush_logfile().");
    }
    if (init_success & FAULT_SCHED_SAMPL) {
        Serial.println("FAULT: failed to schedule sample_sensors().");
    }
#endif
}


void
loop()
{
    /* Only run the loop code if we initialized successfully. */
    if (!init_success) {
        /* Start scheduler. */
        queue.Run(millis());
        delay(TASK_100HZ);
    }
}


/*
 * Open log file and increment the last log number.
 * Returns -EIO if files can't be opened for writing, 0 for success.
 */
int8_t
open_logfile(void)
{
    int8_t rv = TASK_SUCCESS;
    uint8_t num = 0;

    /*
     * Attempt to open the increment file and read in the integer value for
     * the last log file. Note that if the last_log file is empty, the return
     * value will be zero and we just use this as the starting point. If the
     * file doesn't exist, we'll just create it.
     */
    if (SD.exists(INCREMENT_FILE)) {
        if (fd = SD.open(INCREMENT_FILE, FILE_READ)) {
            if (fd.available()) {
                num =fd.parseInt();
            }
            fd.close();
        }
    }

    /* Increment the log file number and write to the increment file. */
    ++num;
    SD.remove(INCREMENT_FILE);
    if (fd = SD.open(INCREMENT_FILE, FILE_WRITE)) {
        fd.print(num);
        fd.close();
    } else {
        /* Failed to open increment file for writing. */
        rv = -EIO;
    }

    /* Open file for logging. */
    String logfn = String(num) + ".csv";
    if (!(fd = SD.open(logfn.c_str(), FILE_WRITE))) {
        /* Failed to open new log file. */
        rv = -EIO;
    }

    return rv;
}


/* Callback function to flush the SD card write buffer. */
int
flush_logfile(unsigned long now)
{
    fd.flush();
    return TASK_SUCCESS;
}


/* Callback function to read in sensor data and write to log file. */
int
sample_sensors(unsigned long now)
{
    /* TODO: Read in analog and PWM signals, log everything. */
    int8_t vn200_rv = vn200.read();
    int8_t uadc_rv = uadc.read();
    float la_pin = analogRead5V(LEFT_AILERON_PIN);
    float ra_pin = analogRead5V(RIGHT_AILERON_PIN);
    float lr_pin = analogRead5V(LEFT_RUDDERVATOR_PIN);
    float rr_pin = analogRead5V(RIGHT_RUDDERVATOR_PIN);
    float lf_pin = analogRead5V(LEFT_FLAP_PIN);
    float rf_pin = analogRead5V(RIGHT_FLAP_PIN);
    fd.println(String(float(millis()) / 1e3, 2) + sep + uadc.airspeed_s() + sep
               + uadc.alpha_s() + sep + uadc.beta_s() + sep + vn200.roll_s()
               + sep + vn200.pitch_s() + sep + vn200.yaw_s() + sep
               + vn200.p_s() + sep + vn200.q_s() + sep + vn200.r_s() + sep
               + throttle_pwm + sep + la_pin + sep + ra_pin + sep + lr_pin
               + sep + rr_pin + sep + lf_pin + sep + rf_pin);
#ifdef DEBUG
    switch (vn200_rv) {
    case -EINACTIVE:
        Serial.println("FAULT: VN-200 serial port inactive.");
        break;
    case -ENODATA:
        Serial.println("WARN : VN-200 serial port has no data available.");
        break;
    case -EINVAL:
        Serial.println("WARN : VN-200 checksum verification failed!");
        break;
    default:
        break;
    }
    switch (uadc_rv) {
    case -EINACTIVE:
        Serial.println("FAULT: UADC serial port inactive.");
        break;
    case -ENODATA:
        Serial.println("WARN : UADC serial port has no data available.");
        break;
    case -EINVAL:
        Serial.println("WARN : UADC checksum verification failed!");
        break;
    default:
        break;
    }
    if (!vn200_rv) {
        Serial.println("VN200: " + vn200.roll_s() + sep + vn200.pitch_s()
                       + sep + vn200.yaw_s() + sep + vn200.p_s() + sep
                       + vn200.q_s() + sep + vn200.r_s());
    }
    if (!uadc_rv) {
        Serial.println("UADC : " + uadc.airspeed_s() + sep + uadc.alpha_s()
                       + sep + uadc.beta_s());
    }
#endif
    if ((vn200_rv < 0) || (uadc_rv < 0)) {
        return TASK_FAILURE;
    }
    return TASK_SUCCESS;
}


/* Interrupt handler for rising throttle PWM signal. */
void
throttle_pwm_rising(void)
{
    attachInterrupt(THROTTLE_INTERRUPT, throttle_pwm_falling, FALLING);
    throttle_last_us = micros();
}


/* Interrupt handler for falling throttle PWM signal. */
void
throttle_pwm_falling(void)
{
    attachInterrupt(THROTTLE_INTERRUPT, throttle_pwm_rising, RISING);
    throttle_pwm = micros() - throttle_last_us;
}


/* Read in voltage level for 5V logic. */
inline float
analogRead5V(uint8_t pin) {
    return analogRead(pin) * 5. / 1023.;
}
