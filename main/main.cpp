#include <Arduino.h>
#include <bsec2.h>
#include <SPI.h>

/* Macros used */
#define PANIC_LED   LED_BUILTIN
#define ERROR_DUR   1000

#define SAMPLE_RATE BSEC_SAMPLE_RATE_LP

// --- SPI Chip Select pin for ESP32-S3 DevKit C ---
#define PIN_CS   10   // Adjust this to the actual CS pin you're using

// Placeholder for temperature offset (define based on testing on your hardware)
#define TEMP_OFFSET_LP   5.0f
#define TEMP_OFFSET_ULP  5.0f

/* Helper functions declarations */
void errLeds(void);
void checkBsecStatus(Bsec2 bsec);
void newDataCallback(const bme68xData data, const bsecOutputs outputs, Bsec2 bsec);

/* Create an object of the class Bsec2 */
Bsec2 envSensor;

/* Entry point for the example */
void setup(void)
{
    /* Desired subscription list of BSEC2 outputs */
    bsecSensor sensorList[] = {
            BSEC_OUTPUT_IAQ,
            BSEC_OUTPUT_RAW_TEMPERATURE,
            BSEC_OUTPUT_RAW_PRESSURE,
            BSEC_OUTPUT_RAW_HUMIDITY,
            BSEC_OUTPUT_RAW_GAS,
            BSEC_OUTPUT_STABILIZATION_STATUS,
            BSEC_OUTPUT_RUN_IN_STATUS,
            BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_TEMPERATURE,
            BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_HUMIDITY,
            BSEC_OUTPUT_STATIC_IAQ,
            BSEC_OUTPUT_CO2_EQUIVALENT,
            BSEC_OUTPUT_BREATH_VOC_EQUIVALENT,
            BSEC_OUTPUT_GAS_PERCENTAGE,
            BSEC_OUTPUT_COMPENSATED_GAS
    };

    Serial.begin(115200);
    pinMode(PANIC_LED, OUTPUT);

    // --- Init SPI bus for ESP32-S3 ---
    SPI.begin( /* SCK = */ 12, /* MISO = */ 13, /* MOSI = */ 11 );  
    pinMode(PIN_CS, OUTPUT);
    digitalWrite(PIN_CS, HIGH);

    /* Valid for boards with USB-COM. Wait until the port is open */
    while(!Serial) delay(10);

    /* Initialize the library and interfaces (SPI version) */
    if (!envSensor.begin(PIN_CS, SPI))
    {
        checkBsecStatus(envSensor);
    }
	
	/*
	 *	The default offset provided has been determined by testing the sensor in LP and ULP mode on application board 3.0
	 *	Please update the offset value after testing this on your product 
	 */
	if (SAMPLE_RATE == BSEC_SAMPLE_RATE_ULP)
	{
		envSensor.setTemperatureOffset(TEMP_OFFSET_ULP);
	}
	else if (SAMPLE_RATE == BSEC_SAMPLE_RATE_LP)
	{
		envSensor.setTemperatureOffset(TEMP_OFFSET_LP);
	}

    /* Subscribe to the desired BSEC2 outputs */
    if (!envSensor.updateSubscription(sensorList, ARRAY_LEN(sensorList), SAMPLE_RATE))
    {
        checkBsecStatus(envSensor);
    }

    /* Whenever new data is available call the newDataCallback function */
    envSensor.attachCallback(newDataCallback);

    Serial.println("BSEC library version " + \
            String(envSensor.version.major) + "." \
            + String(envSensor.version.minor) + "." \
            + String(envSensor.version.major_bugfix) + "." \
            + String(envSensor.version.minor_bugfix));
}

/* Function that is looped forever */
void loop(void)
{
    if (!envSensor.run())
    {
        checkBsecStatus(envSensor);
    }
}

void errLeds(void)
{
    while(1)
    {
        digitalWrite(PANIC_LED, HIGH);
        delay(ERROR_DUR);
        digitalWrite(PANIC_LED, LOW);
        delay(ERROR_DUR);
    }
}

void newDataCallback(const bme68xData data, const bsecOutputs outputs, Bsec2 bsec)
{
    if (!outputs.nOutputs)
    {
        return;
    }

    Serial.println("BSEC outputs:\n\tTime stamp = " + String((int) (outputs.output[0].time_stamp / INT64_C(1000000))));
    for (uint8_t i = 0; i < outputs.nOutputs; i++)
    {
        const bsecData output  = outputs.output[i];
        switch (output.sensor_id)
        {
            case BSEC_OUTPUT_IAQ:
                Serial.println("\tIAQ = " + String(output.signal));
                Serial.println("\tIAQ accuracy = " + String((int) output.accuracy));
                break;
            case BSEC_OUTPUT_RAW_TEMPERATURE:
                Serial.println("\tTemperature = " + String(output.signal));
                break;
            case BSEC_OUTPUT_RAW_PRESSURE:
                Serial.println("\tPressure = " + String(output.signal));
                break;
            case BSEC_OUTPUT_RAW_HUMIDITY:
                Serial.println("\tHumidity = " + String(output.signal));
                break;
            case BSEC_OUTPUT_RAW_GAS:
                Serial.println("\tGas resistance = " + String(output.signal));
                break;
            case BSEC_OUTPUT_STABILIZATION_STATUS:
                Serial.println("\tStabilization status = " + String(output.signal));
                break;
            case BSEC_OUTPUT_RUN_IN_STATUS:
                Serial.println("\tRun in status = " + String(output.signal));
                break;
            case BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_TEMPERATURE:
                Serial.println("\tCompensated temperature = " + String(output.signal));
                break;
            case BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_HUMIDITY:
                Serial.println("\tCompensated humidity = " + String(output.signal));
                break;
            case BSEC_OUTPUT_STATIC_IAQ:
                Serial.println("\tStatic IAQ = " + String(output.signal));
                break;
            case BSEC_OUTPUT_CO2_EQUIVALENT:
                Serial.println("\tCO2 Equivalent = " + String(output.signal));
                break;
            case BSEC_OUTPUT_BREATH_VOC_EQUIVALENT:
                Serial.println("\tbVOC equivalent = " + String(output.signal));
                break;
            case BSEC_OUTPUT_GAS_PERCENTAGE:
                Serial.println("\tGas percentage = " + String(output.signal));
                break;
            case BSEC_OUTPUT_COMPENSATED_GAS:
                Serial.println("\tCompensated gas = " + String(output.signal));
                break;
            default:
                break;
        }
    }
}

void checkBsecStatus(Bsec2 bsec)
{
    if (bsec.status < BSEC_OK)
    {
        Serial.println("BSEC error code : " + String(bsec.status));
        errLeds(); /* Halt in case of failure */
    }
    else if (bsec.status > BSEC_OK)
    {
        Serial.println("BSEC warning code : " + String(bsec.status));
    }

    if (bsec.sensor.status < BME68X_OK)
    {
        Serial.println("BME68X error code : " + String(bsec.sensor.status));
        errLeds(); /* Halt in case of failure */
    }
    else if (bsec.sensor.status > BME68X_OK)
    {
        Serial.println("BME68X warning code : " + String(bsec.sensor.status));
    }
}

// Define ARRAY_LEN macro if not defined
#ifndef ARRAY_LEN
#define ARRAY_LEN(array) (sizeof(array)/sizeof(array[0]))
#endif

// Main app_main function for ESP-IDF compatibility
extern "C" void app_main(void) {
    // Initialize Arduino
    initArduino();
    
    // Call Arduino's setup
    setup();
    
    // Run Arduino's loop in a task
    while (true) {
        loop();
        // Add a small delay to prevent watchdog resets
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
