#include "bmv080_wrapper.h"
#include "esphome/core/log.h"

extern "C" {
    #include "bmv080.h"
    #include "bmv080_defs.h"
}

namespace esphome {
namespace bmv080 {

static const char *const TAG = "bmv080.sensor";

int8_t BMV080Sensor::bmv080_i2c_read(bmv080_sercom_handle_t sercomHandle, uint16_t header, uint16_t* payload, uint16_t len) {
    auto* device = reinterpret_cast<BMV080Sensor*>(sercomHandle);
    uint8_t address = device->address_;

    uint16_t header_adjusted = header << 1;
    uint8_t header_bytes[2] = { 
        (uint8_t)(header_adjusted >> 8),
        (uint8_t)(header_adjusted & 0xFF)
    };
    
    uint8_t buffer[1024];
    if (len > 512) {
        ESP_LOGE(TAG, "Read length %u exceeds buffer", len);
        return -1;
    }

    if (device->bus_->write(address, header_bytes, 2, true) != i2c::ERROR_OK) {
        return -1;
    }

    if (device->bus_->read(address, buffer, len * 2) != i2c::ERROR_OK) {
        return -1;
    }

    for (size_t i = 0; i < len; i++) {
        payload[i] = (uint16_t(buffer[i * 2]) << 8) | uint16_t(buffer[i * 2 + 1]);
    }
    
    return 0;
}

int8_t BMV080Sensor::bmv080_i2c_write(bmv080_sercom_handle_t sercomHandle, uint16_t header, const uint16_t* payload, uint16_t len) {
    auto* device = reinterpret_cast<BMV080Sensor*>(sercomHandle);
    uint8_t address = device->address_;

    uint16_t header_adjusted = header << 1;
    
    uint8_t data[1026];
    size_t data_len = 2 + len * 2;
    
    if (data_len > 1026) {
        ESP_LOGE(TAG, "Write length %u exceeds buffer", len);
        return -1;
    }
    
    data[0] = (uint8_t)(header_adjusted >> 8);
    data[1] = (uint8_t)(header_adjusted & 0xFF);

    for (size_t i = 0; i < len; i++) {
        data[2 + i * 2] = (uint8_t)(payload[i] >> 8);
        data[2 + i * 2 + 1] = (uint8_t)(payload[i] & 0xFF);
    }

    if (device->bus_->write(address, data, data_len, true) != i2c::ERROR_OK) {
        return -1;
    }
    
    return 0;
}

int8_t BMV080Sensor::bmv080_delay(uint32_t period_ms) {
    vTaskDelay(pdMS_TO_TICKS(period_ms)); 
    return 0;
}

void bmv080_on_data_ready(bmv080_output_t output, void* callback_parameters) {
    auto *sensor = (BMV080Sensor *) callback_parameters;
    sensor->publish_state(output.pm2_5_mass_concentration);
    ESP_LOGI(TAG, "Is Obstructed: %s", output.is_obstructed ? "YES" : "NO");
    ESP_LOGI(TAG, "PM2.5: %.2f µg/m³ PM10: %.2f µg/m³", output.pm2_5_mass_concentration, output.pm10_mass_concentration);
}

void BMV080Sensor::sensor_task(void* parameter) {
    auto* sensor = (BMV080Sensor*)parameter;
    
    ESP_LOGI(TAG, "BMV080 Task gestartet");
    
    while (true) {
        if (sensor->sensorHandle != nullptr) {
            bmv080_serve_interrupt(sensor->sensorHandle, bmv080_on_data_ready, sensor);
        }
        
        UBaseType_t stack_free = uxTaskGetStackHighWaterMark(NULL);
        ESP_LOGD(TAG, "Stack frei: %u bytes", stack_free * 4);
        
        vTaskDelay(pdMS_TO_TICKS(10000));
    }
}

void BMV080Sensor::setup() {
    ESP_LOGI(TAG, "Initialisiere BMV080...");
    
    uint16_t major = 0;
    uint16_t minor = 0;
    uint16_t patch = 0;
    char git_hash[12];
    int32_t commits_ahead = 0;
    bmv080_status_code_t status = E_BMV080_OK;

    status = bmv080_get_driver_version(&major, &minor, &patch, git_hash, &commits_ahead);
    
    if (status != E_BMV080_OK) {
        ESP_LOGE(TAG, "Version check fehlgeschlagen: %d", status);
        this->mark_failed();
        return;
    }

    ESP_LOGI(TAG, "SDK Version: %u.%u.%u.%s.%ld", major, minor, patch, git_hash, (long)commits_ahead);
    
    status = bmv080_open(&this->sensorHandle, this, bmv080_i2c_read, bmv080_i2c_write, bmv080_delay);

    if (status == E_BMV080_OK) {
        ESP_LOGI(TAG, "Bosch SDK erfolgreich geladen");
        bmv080_start_continuous_measurement(this->sensorHandle);
        
        BaseType_t task_created = xTaskCreate(
            sensor_task,           
            "bmv080_task",        
            60 * 1024,
            this,                 
            configMAX_PRIORITIES - 1,
            &sensor_task_handle_  
        );
        
        if (task_created == pdPASS) {
            ESP_LOGI(TAG, "BMV080 Task mit 60KB Stack gestartet");
        } else {
            ESP_LOGE(TAG, "Task-Erstellung fehlgeschlagen");
            this->mark_failed();
        }
    } else {
        ESP_LOGE(TAG, "SDK Init fehlgeschlagen: %d", status);
        this->mark_failed();
    }
}


void BMV080Sensor::dump_config() {
    ESP_LOGCONFIG(TAG, "BMV080 Sensor:");
    LOG_I2C_DEVICE(this);
}

}
}