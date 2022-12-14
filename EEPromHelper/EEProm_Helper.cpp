#include "EEProm_Helper.h"

typedef struct {
    uint32_t checksum;
    uint32_t version;
    uint32_t dataSize;
} PrefHeader;

const uint16_t headerAddr = 0;
const uint16_t dataAddr = headerAddr + sizeof(PrefHeader);

EEPromHelper::EEPromHelper() {
    _address = 0;
    EEPROM.begin(512);
    _numBytes = EEPROM.length();
}

#ifdef USE_I2C_EEPROM
EEPromHelper::EEPromHelper(uint8_t address, uint32_t numBytes) {
    _address = address;
    _numBytes = numBytes;
}
#endif

bool EEPromHelper::begin() {
#ifdef USE_I2C_EEPROM
    if (_address) {
        _eeprom = new I2C_eeprom(_address, _numBytes);

        if (_eeprom) {
            return _eeprom->begin();
        }
        else {
            return false;
        }
    }
    else {
        return true;
    }
#else
    return true;
#endif
}

bool EEPromHelper::setPrefs(void* data, uint32_t sizeInBytes, uint32_t version) {
    _data = data;
    _sizeInBytes = sizeInBytes;
    _version = version;

    return readData(_data, _sizeInBytes, _version);
}

bool EEPromHelper::writePrefs(bool force) {
    if (!force && _doDelay) {
        _dirty = true;
        _dirtyTime = 0;
        return true;
    }
    else {
        _dirty = !updateData(_data, _sizeInBytes, _version);
        return !_dirty;
    }
}

bool EEPromHelper::update() {
    bool result = false;

    if (_dirty && _dirtyTime >= _waitTime) {
        result = writePrefs(true);
    }

    return result;
}

bool EEPromHelper::readData(void* data, uint32_t sizeInBytes, uint32_t version) {
    bool result = false;
    PrefHeader storedHeader;

    if (verifiedReadBlock(headerAddr, &storedHeader, sizeof(storedHeader))) {
        if (version == storedHeader.version && sizeInBytes == storedHeader.dataSize) {
            if (verifiedReadBlock(dataAddr, data, sizeInBytes)) {
                if (generateChecksum(data, sizeInBytes) == storedHeader.checksum) {
                    Serial.println("EEPromHelper::readData - checksum matches");
                    result = true;
                }
            }
        }
    }
    return result;
}

bool EEPromHelper::updateData(void* data, uint32_t sizeInBytes, uint32_t version) {
    bool result = false;
    PrefHeader header;

    Serial.println("updateData");

    header.checksum = generateChecksum(data, sizeInBytes);
    header.version = version;
    header.dataSize = sizeInBytes;

    if (verifiedUpdateBlock(headerAddr, &header, sizeof(header))) {
        Serial.println("Header written");
        result = verifiedUpdateBlock(dataAddr, data, sizeInBytes);
        Serial.println("Data written");
    }

    return result;
}

uint32_t EEPromHelper::appendToChecksum(uint32_t checksum, void* data, uint32_t sizeInBytes) {
    uint8_t* dataPtr = (uint8_t*)data;

    for (uint32_t i=0; i<sizeInBytes; i++) {
        checksum = (((checksum << 1) | (checksum >> 31)) ^ 0xAAAAAAAA) ^ dataPtr[i];
    }

    return checksum;
}

uint32_t EEPromHelper::generateChecksum(void* data, uint32_t sizeInBytes) {
    return appendToChecksum(0, data, sizeInBytes);
}

bool EEPromHelper::verifiedReadBlock(uint16_t memoryAddress, void* data, uint32_t sizeInBytes) {
#ifdef USE_I2C_EEPROM
    if (_eeprom) {
        return (_eeprom->readBlock(memoryAddress, (uint8_t*)data, sizeInBytes) == (int)sizeInBytes);
    }
    else 
#endif
    if (memoryAddress+sizeInBytes <= _numBytes) {
        uint8_t* buffer = (uint8_t*)data;
        while (sizeInBytes--) {
            *(buffer++) = EEPROM.read(memoryAddress++);
        }
        return true;
    }
    else {
        return false;
    }
}

bool EEPromHelper::verifiedUpdateBlock(uint16_t memoryAddress, void* data, uint32_t sizeInBytes) {
#ifdef USE_I2C_EEPROM
    if (_eeprom) {
        return (_eeprom->updateBlock(memoryAddress, (uint8_t*)data, sizeInBytes) == (int)sizeInBytes);
    }
    else
#endif
    Serial.print("writing bytes: addr=");
    Serial.print(memoryAddress);
    Serial.print(", sizeInBytes=");
    Serial.print(sizeInBytes);
    Serial.print(", _numBytes=");
    Serial.println(_numBytes);
    if (memoryAddress+sizeInBytes <= _numBytes) {
       uint8_t* buffer = (uint8_t*)data;
        while (sizeInBytes--) {
            if (EEPROM.read(memoryAddress) != *buffer) {
                Serial.print("Writing byte at ");
                Serial.println(memoryAddress);
                EEPROM.write(memoryAddress, *buffer);
            }
            memoryAddress++;
            buffer++;
        }
        return true;
    }
    else {
        Serial.println("Failed!");
        return false;
    }
}
