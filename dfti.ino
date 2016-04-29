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
#define SD_CARD     4
#define FN_BUFLEN  50
/* Increment file to record the last log file number. */
#define INCREMENT_FILE "lastlog.txt"
/* Configure analog inputs. */
#define LEFT_AILERON_PIN      1
#define RIGHT_AILERON_PIN     2
#define LEFT_RUDDERVATOR_PIN  3
#define RIGHT_RUDDERVATOR_PIN 4
#define LEFT_FLAP_PIN         5
#define RIGHT_FLAP_PIN        6
/* Define rates for scheduler. */
#define TASK_SUCCESS     0
#define TASK_FAILURE     1
#define TASK_30SEC   30000
#define TASK_10SEC   10000
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
    vn200 = VN200(&Serial2, (uint8_t) VN200_BAUD_RATE);
    uadc = UADC(&Serial3, (uint8_t) UADC_BAUD_RATE);
    vn200.begin();
    uadc.begin();

    /* Attempt to open the SD card and open the log file. */
    if (!SD.begin(SD_CARD)) {
        init_success ^= FAULT_SDIO;
    } else {
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
    String logfn = String(num) + ".txt";
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
    if (fd != NULL) {
        fd.flush();
    }
    return TASK_SUCCESS;
}


/* Callback function to read in sensor data and write to log file. */
int
sample_sensors(unsigned long now)
{
    /* TODO: Read in analog and PWM signals, log everything. */
    vn200.read();
    uadc.read();
#ifdef DEBUG
    Serial.println(String("VN200: ") + vn200.roll_s() + sep + vn200.pitch_s()
                   + sep + vn200.yaw_s() + sep + vn200.p_s() + sep
                   + vn200.q_s() + sep + vn200.r_s());
    Serial.println(String("UADC : ") + sep + uadc.airspeed_s() + sep
                   + uadc.alpha_s() + sep + uadc.beta_s());
#endif
    fd.println(String("VN200: ") + vn200.roll_s() + sep + vn200.pitch_s()
               + sep + vn200.yaw_s() + sep + vn200.p_s() + sep + vn200.q_s()
               + sep + vn200.r_s());
    fd.println(String("UADC : ") + sep + uadc.airspeed_s() + sep
               + uadc.alpha_s() + sep + uadc.beta_s());
    return TASK_SUCCESS;
}
