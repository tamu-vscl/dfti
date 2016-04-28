/*
 *  VSCL Developmental Flight Test Instrumentation Code
 *
 *  Reads in sensor values and stores to file.
 */

/* Arduino stdlib */
#include <SD.h>
/* Third-party libs */
#include <errno.h>
#include <Queue.h>
/* Custom libs */
#include <libadc.hh>
#include <libvn200.hh>


/* Configure serial devices. */
#define VN200_BAUD_RATE 115200
#define UADC_BAUD_RATE 115200
/* Configure SD card logging. */
#define SD_CARD 8
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


/* Global variables. */
/* File object for log. */
File logfile = NULL;
/* Sensor objects. */
VN200 vn200(&Serial2, VN200_BAUD_RATE);
ADC uadc(&Serial3, UADC_BAUD_RATE);
/* Task scheduling queue. */
Queue queue;
/* Flag to ensure successful initialization. */
bool init_success = true;


void
setup()
{
    /* Setup h/w serial devices. */
    vn200.begin();
    uadc.begin();

    /* Attempt to open the SD card and open the log file. */
    if (!SD.begin(SD_CARD)) {
        init_success = false;
    } else {
        if (open_logfile() < 0) {
            init_success = false;
        }
    }

    /* Flush data to file every 10 seconds. */
    queue.scheduleFunction(flush_logfile, "FLUSHLOG", 0, TASK_10SEC);
    /* Read in sensor data at 100 Hz and log. */
    queue.scheduleFunction(sample_sensors, "SAMPDATA", 0, TASK_100HZ);
}


void
loop()
{
    /* Only run the loop code if we initialized successfully. */
    if (init_success) {
        /* Start scheduler. */
        queue.Run(millis());
        delay(TASK_100HZ);
    }
}


/*
 * Open log file and increment the last log number.
 * Returns -EIO if files can't be opened for writing, 0 for success.
 */
uint8_t
open_logfile(void)
{
    int8_t rv = TASK_SUCCESS;
    uint8_t num = 0;
    File log = NULL;

    /*
     * Attempt to open the increment file and read in the integer value for
     * the last log file. Note that if the last_log file is empty, the return
     * value will be zero and we just use this as the starting point. If the
     * file doesn't exist, we'll just create it.
     */
    if SD.exists(INCREMENT_FILE) {
        log = SD.open(INCREMENT_FILE, FILE_READ);
        if (log.available()){
            num = log.parseInt();
        }
        log.close();
    }

    /* Increment the log file number and write to the increment file. */
    ++num;
    log = SD.open(INCREMENT_FILE, FILE_WRITE);
    if (!log) {
        /* Failed to open increment file for writing. */
        rv = -EIO;
    }
    log.write(num);
    log.close();
    log = NULL;

    /* Open file for logging. */
    logfile = open(sprintf("flightlog%03d.txt", num), FILE_WRITE);
    if (!logfile) {
        /* Failed to open new log file. */
        rv = -EIO;
    }

    return rv;
}


/* Callback function to flush the SD card write buffer. */
int
flush_logfile(unsigned int now)
{
    if (logfile != NULL) {
        logfile.flush();
    }
    return TASK_SUCCESS;
}


/* Callback function to read in sensor data and write to log file. */
int
sample_sensors(unsigned int now)
{
    /* TODO: Read in analog and PWM signals, log everything. */
    vn200.read();
    uadc.read();
    return TASK_SUCCESS;
}
