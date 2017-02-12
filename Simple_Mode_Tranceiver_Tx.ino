#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"


RF24 radio(9,10);
// Pipes for reading/writing { Controller, Head, Body }
const uint64_t pipes[2] = { 0xF0F0F0F0D2LL, 0xF0F0F0F0C3LL};

typedef struct{
  int Gas;
  int Ail;
  int Prof;
  int Deriv;
  long timestamp;
}
tD;
tD transmit_data;
tD receive_data;

void setup(void)
{
    Serial.begin(115200);
    Serial.println("Manette");

    radio.begin();
    radio.setPALevel(RF24_PA_LOW);
    radio.setRetries(15, 15);
    
    radio.openWritingPipe(pipes[0]);
    radio.openReadingPipe(1, pipes[1]); // Head
    radio.startListening();
    //radio.printDetails();
}

void loop(void)
{
    // First, stop listening so we can talk.
    radio.stopListening();

    // Take the time, and send it.  This will block until complete
    unsigned long time = millis();
    Serial.print("Now sending ");
    transmit_data.Gas = 10;
    transmit_data.Ail = 20;
    transmit_data.Prof = 30;
    transmit_data.Deriv = 40;
    transmit_data.timestamp = millis();
    bool ok = radio.write( &transmit_data, sizeof(transmit_data) );
    
    if (ok)
        Serial.print("ok...");
    else
        Serial.println("failed!");

    // Now, continue listening
    radio.startListening();
    
    bool timeout = false;
    uint8_t pipe_num;
    while ( ! radio.available(&pipe_num) && ! timeout )
        if (millis() - transmit_data.timestamp > 250 )
            timeout = true;


    
    if ( timeout ) {
        Serial.println("Failed, response timed out.");
    } else {
        radio.read( &receive_data, sizeof(receive_data) );
        
        Serial.print("Gas" );         Serial.print(receive_data.Gas   );
        Serial.print("\tAileron" );   Serial.print(receive_data.Ail   );
        Serial.print("\tProfondeur" );Serial.print(receive_data.Prof  );
        Serial.print("\tDeriv");      Serial.print(receive_data.Deriv );
        Serial.print("\t");      Serial.print(receive_data.timestamp );
    }
    Serial.println("");
    
    delay(10);
}
