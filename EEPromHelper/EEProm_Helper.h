#ifndef _EEPROM_HELPER_
#define _EEPROM_HELPER_

#include "EEPROM.h"
#include "elapsedMillis.h"

#ifdef USE_I2C_EEPROM
#include "I2C_eeprom.h"
#endif

class EEPromHelper {
    public:
        EEPromHelper();
#ifdef USE_I2C_EEPROM
        EEPromHelper(uint8_t address = 0x57, uint32_t numBytes = 4096);
#endif
        bool begin();
        bool setPrefs(void* data, uint32_t sizeInBytes, uint32_t version);
        bool writePrefs(bool force = false);

        void setDelayedWrite(bool doDelay) { _doDelay = true; };
        void setDelayTime(uint32_t waitTime) { _waitTime = waitTime; };
        bool update();

    private:
        uint8_t _address;
        uint32_t _numBytes;
#ifdef USE_I2C_EEPROM
        I2C_eeprom* _eeprom;
#endif

        void* _data;
        uint32_t _sizeInBytes;
        uint32_t _version;

        bool _doDelay;
        bool _dirty;
        elapsedMillis _dirtyTime;
        uint32_t _waitTime = 1000;

        bool readData(void* data, uint32_t sizeInBytes, uint32_t version);
        bool updateData(void* data, uint32_t sizeInBytes, uint32_t version);

        uint32_t generateChecksum(void* data, uint32_t sizeInBytes);
        uint32_t appendToChecksum(uint32_t checksum, void* data, uint32_t sizeInBytes);

        bool verifiedReadBlock(uint16_t memoryAddress, void* data, uint32_t sizeInBytes);
        bool verifiedUpdateBlock(uint16_t memoryAddress, void* data, uint32_t sizeInBytes);
};


#endif
