#include <BluetoothA2DPCommon.h>
#include <BluetoothA2DPSink.h>
#include <BluetoothA2DPSource.h>
#include <SoundData.h>
#include <stdio.h>
#include <string.h>



BluetoothA2DPSink a2dp_sink;

void setup() {
// play/pause sensor
pinMode(18, INPUT);
// metadata serial AVRC
Serial.begin(115200);
a2dp_sink.set_avrc_metadata_callback(avrc_metadata_callback);
// I2S audio setup
static const i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
    .sample_rate = 44100,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
    .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
    .communication_format = (i2s_comm_format_t)(I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB),
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 4,                               // 4x buffer
    .dma_buf_len = 768,                               // 768bits / buffer
    .use_apll=true,
    .tx_desc_auto_clear= true, 
    .fixed_mclk=0
};
// I2S pin setup
static const i2s_pin_config_t pin_config = {
    .bck_io_num = 27,                                 // Bitclock P27
    .ws_io_num = 26,                                  // Wordclock P26
    .data_out_num = 22,                               // Data P22
    .data_in_num = I2S_PIN_NO_CHANGE                  // NC
};
// I2S init
    a2dp_sink.set_i2s_config(i2s_config);
    a2dp_sink.set_pin_config(pin_config);
    a2dp_sink.start("Radio Alice");
}
// Metadata retrieve, print to serial
void avrc_metadata_callback(uint8_t id, const uint8_t *text){
  if(id == 0x2){
    Serial.printf("%s\n", text);
  }
} 
// Main Variables
int toggle, new_state = 0;
// Main code
void loop() {
  // read sensor state  
  toggle = digitalRead(18);
  // if decection = true
  if(toggle == HIGH){
    // pause if playing
    if(new_state == 0){
      a2dp_sink.pause();
      new_state = 1;
      Serial.print("HATS\n");
    }
    // play if paused
    else if(new_state == 1){
      a2dp_sink.play();
      new_state = 0;
      Serial.print("HUTS\n");
    }
  // 1 sec delay      
  delay(1000);
  }
}

/*

dependencies: ESP-IDF from espressif & A2DP-Master from P Schatzman

direct memory acces buffer:
buffer size >
(16bit*2stereo/4buffer)*samplerate =176400
I2S buffer max length in  bits 1024 and at least 8 bits
formule >
(bits per sample / 8 * channel count * dma buffer length * dma buffer count)
(16/8)*2stereo*2length=8kbyte

*/
