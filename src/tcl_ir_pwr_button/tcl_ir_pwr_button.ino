#include <IRremote.h>

#define PIN_IR_RECV 2
#define PIN_IR_SEND 3
#define PIN_STATUS_LED 4

#define FREQ_CARRIER_KHZ 38
#define CREATIVE_REMOTE_DATA 0xEC13A4BF
#define DELAY_MS_LOOP 5

/* Sequence in us of the TCL power on command to send */
static uint16_t tclPowerOnRawData[] = {
    /* Command */
    4039, 3948,  619, 1901, 552, 1958,  443, 2021, 619, 1878,  575,  959, 525, 998, 558, 1879, 576,
     922,  620, 1912,  584, 901,  597, 1902,  596, 941,  498,  961,  595, 881, 617, 881,  618, 880,
     601, 1896,  617, 1881, 618,  882,  612, 1885, 616,  882,  616, 1881, 616, 944, 497, 1939, 614,
    /* Gap */
    8511,
    /* Repeat */
    4039, 3948,  619, 1901, 552, 1958,  443, 2021, 619, 1878, 575,  959, 525, 998, 558, 1879, 576,
     922,  620, 1912, 584,  901,  597, 1902,  596, 941,  498, 961,  595, 881, 617, 881,  618, 880,
     601, 1896,  617, 1881, 618,  882,  612, 1885, 616,  882, 616, 1881, 616, 944, 497, 1939, 614
};

void setup()
{
    pinMode(PIN_STATUS_LED, OUTPUT);
    digitalWrite(PIN_STATUS_LED, LOW);

    IrSender.begin(PIN_IR_SEND);
    IrReceiver.begin(PIN_IR_RECV, true);
}

void loop()
{
    if (IrReceiver.decode()) {
        if (IrReceiver.decodedIRData.decodedRawData == CREATIVE_REMOTE_DATA) {
            IrSender.sendRaw(tclPowerOnRawData, sizeof(tclPowerOnRawData)/sizeof(uint16_t), FREQ_CARRIER_KHZ);
            digitalWrite(PIN_STATUS_LED, !digitalRead(PIN_STATUS_LED));
            delay(DELAY_MS_LOOP);
        }
        IrReceiver.resume(); // Receive the next IR signal
    }
}
