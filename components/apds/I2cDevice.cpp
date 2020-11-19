#include "I2cDevice.h"
#include "driver/i2c.h"
#include "esp_log.h"

#define ACK_VAL    (i2c_ack_type_t)0x0
#define NACK_VAL   (i2c_ack_type_t)0x1
#define I2C_PORT I2C_NUM_1
#define I2C_FREQUENCY 100000
#define APDS_I2C_ADDRESS 0x39

#define DEBUG 0

I2cDevice::I2cDevice() {
};

I2cDevice::~I2cDevice() {
};

bool I2cDevice::i2cInit(void) {
    i2c_config_t conf;
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = (gpio_num_t)CONFIG_SDA;
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf.scl_io_num = (gpio_num_t)CONFIG_SCL;
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    conf.master.clk_speed = I2C_FREQUENCY;
    ESP_ERROR_CHECK(i2c_param_config(I2C_PORT, &conf));
    ESP_ERROR_CHECK(i2c_driver_install(I2C_PORT, conf.mode, 0, 0, 0));

    return true;
}

void I2cDevice::delay(long millis) {
    #if DEBUG
        ESP_LOGI("I2C", "delay: %ld", millis);
    #endif
    
    vTaskDelay(millis / portTICK_RATE_MS);
}

bool I2cDevice::writeByte(uint8_t val) {
    return writeDataBlock(APDS_I2C_ADDRESS, &val, 1);
}

bool I2cDevice::writeDataByte(uint8_t reg, uint8_t val) {
    #if DEBUG
        ESP_LOGI("I2C", "write data %x, to register %x", val, reg);
    #endif
    
    return writeDataBlock(reg, &val, 1);
}

bool I2cDevice::writeDataBlock(uint8_t reg, uint8_t *val, unsigned int len) {
    esp_err_t ret;
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (APDS_I2C_ADDRESS << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, reg, true);
    i2c_master_write(cmd, val, len, true);
    ret = i2c_master_cmd_begin(I2C_PORT, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
    return ret == ESP_OK;
}

bool I2cDevice::readDataByte(uint8_t reg, uint8_t &val) {
    return readDataBlock(reg, &val, 1) > 0;
}

int I2cDevice::readDataBlock(uint8_t reg, uint8_t *val, unsigned int len) {
    esp_err_t ret;
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (APDS_I2C_ADDRESS << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, reg, true);
    i2c_master_stop(cmd);
    ret = i2c_master_cmd_begin(I2C_PORT, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
    if (ret == ESP_FAIL) {
        return -1;
    }
    cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (APDS_I2C_ADDRESS << 1) | I2C_MASTER_READ, true);
    if (len > 1) {
        i2c_master_read(cmd, val, len - 1, ACK_VAL);
        i2c_master_read_byte(cmd, val + len - 1, NACK_VAL);
    } else {
        i2c_master_read_byte(cmd, val, NACK_VAL);
        #if DEBUG
            ESP_LOGI("I2C", "data read: %x on register %x", *val, reg);
        #endif
    }
    i2c_master_stop(cmd);
    ret = i2c_master_cmd_begin(I2C_PORT, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
    if (ret == ESP_FAIL) {
        return -1;
    }

    return len;
}