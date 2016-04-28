/*
 *  VSCL Developmental Flight Test Instrumentation Code
 *
 *  Reads in sensor values and stores to file.
 */

// Arduino stdlib
#include <SPI.h>
#include <SD.h>
// Third-party libs
#include <Queue.h>
// Custom libs
#include <errno.h>
#include <libbc.hh>
#include <libvn200.hh>


/* Configure serial devices. */
#define VN200_BAUD_RATE 115200
#define UADC_BAUD_RATE 115200
/* Configure SD card logging. */
#define FLUSH_THRESHOLD 10000
#define SD_CARD             4
// Increment file to record the last log file number.
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
// Aliases for h/w serial ports to allow easily reconfiguration.
HardwareSerial *VN200Serial = &Serial2;
HardwareSerial *ADCSerial = &Serial3;
// File object for log.
File logfile = NULL;
// Sensor objects.
VN200 vn200(&Serial2, VN200_BAUD_RATE);
// Task scheduling queue.
Queue queue;
// Flag to ensure successful initialization.
uint8_t init_success = true;


void
setup()
{
    // Setup h/w serial devices.
    vn200.begin();
    ADCSerial->begin(UADC_BAUD_RATE);

    // Attempt to open the SD card and open the log file.
    if (!SD.begin(SD_CARD)) {
        init_success = false;
    } else {
        if (setup_logfile() < 0) {
            init_success = false;
        }
    }

    // Clear buffers.
    memset(vn200_buf, 0, VN200_BUFSIZE);
    memset(vn200data, 0, sizeof(struct VN200Data));

    // Set up task scheduling.
    // Flush data to file every FLUSH_THRESHOLD seconds.
    queue.scheduleFunction(flush_logfile, "WRTLOG", 0, TASK_10SEC);
    // Read in sensor data at 100 Hz.
    queue.scheduleFunction(read_sensors, "GETDATA", 0, TASK_100HZ);
}


void
loop()
{
    // Only run the loop code if we initialized successfully.
    if (init_success) {
        // Start scheduler.
        queue.Run(millis());
        delay(TASK_100HZ);
    }
}


DFTIError
setup_logfile(void)
{
    int8_t rv = 0;
    uint8_t num = 0;
    File last_log, current_log;

    if SD.exists(INCREMENT_FILE) {
        /* Open the increment file and read in the integer value for the last
         * log file. Note that if the last_log file is empty, the return value
         * will be zero and we just use this as the starting point.
         */
        last_log = SD.open(INCREMENT_FILE, FILE_READ);
        if (last_log.available()){
            num = last_log.parseInt();
        }
        last_log.close()
        // Increment the log file number and write to the increment file.
        ++num;
        last_log = SD.open(INCREMENT_FILE, FILE_WRITE);
        last_log.write(num);
        last_log.close();
        // Open file for logging.
        logfile = open(sprintf("flightlog%03d.txt", num), FILE_WRITE);
        if (!logfile) {
            rv = -EIO;
        }
        // Set the pointer to the current log file.
        return 0;
    } else {
        rv = -ENOENT;
    }

}


// Callback function to flush the SD card write buffer.
int
flush_logfile(unsigned int now)
{
    if (logfile != NULL) {
        logfile.flush();
    }
}


int
read_sensors(unsigned int now)
{
    // TODO: Call the sensor object read() methods.
    return TASK_SUCCESS;
}


int
read_adc(unsigned int now)
{
    if (ADCSerial->available()) {
        // TODO: Read in serial data.
    }
    return TASK_SUCCESS;
}


void
parse_vn200(void)
{
    vn200data.yaw = bytes2float(vn200_buf, 4));
    vn200data.pitch = bytes2float(vn200_buf, 8));
    vn200data.roll = bytes2float(vn200_buf, 12));
    vn200data.p = bytes2float(vn200_buf, 28));
    vn200data.q = bytes2float(vn200_buf, 32));
    vn200data.r = bytes2float(vn200_buf, 36));
}


float bytes2float(uint8_t *buf, uint8_t idx)
{
    // Assumes a 4-byte float representation.
    uint8_t b[4] = {buf[idx + 3], buf[idx + 2], buf[idx + 1], buf[idx]};
    float rv;
    memset(&rv, &b, sizeof(float));
    return rv;
}


float
deg2rad(float d)
{
    return PI / 180 * d;
}


// TODO; Need to make sure we use the correct buffer parts.
bool
valid_vn200_pkt(uint8_t buf[], uint8_t len)
{
    /* Warning: FM here. */
    uint16_t crc = 0x0000;
    for (uint8_t i = 0; i < len; ++i) {
        crc = (uint16_t)((uint8_t) (crc >> 8) | (crc << 8));
        crc ^= (uint16_t) buf[i];
        crc ^= (uint8_t) (crc & 0xff) >> 4;
        crc ^= crc << 12;
        crc ^= (crc & 0x00ff) << 5;
    }
    return crc == 0x0000 ? true : false;
}
