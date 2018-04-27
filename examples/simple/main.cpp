#include <Arduino.h>
#include <SPI.h>
#include <lwstreams/streams.h>

void setup() {
    Serial.begin(119200);

    while (!Serial) {
        delay(100);
    }
}

void loop() {

}
