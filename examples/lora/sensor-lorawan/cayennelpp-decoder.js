// Based on https://github.com/HAN-IoT-LAB/TTN-Decoder/blob/main/decoder_cayenneLPP_extreme.js
// but updated to match CayenneLPP TTN uses

/* This code is free software:
 * you can redistribute it and/or modify it under the terms of a Creative
 * Commons Attribution-NonCommercial 4.0 International License
 * (http://creativecommons.org/licenses/by-nc/4.0/)
 *
 * Copyright (c) 2024 March by Klaasjan Wagenaar, Tristan Bosveld and Richard Kroesen
 */

/**
 * @brief Definitions for sensor types used in IoT device data decoding.
 *
 * This file contains the `SensorTypes` constant, which is a mapping of various sensor types
 * to their respective characteristics. Each sensor type is an object with the following properties:
 * - `type`: Numeric identifier for the sensor type.
 * - `precision`: The factor by which the raw sensor data is divided to obtain a meaningful value.
 * - `signed`: Boolean indicating if the sensor data is signed (true) or unsigned (false).
 * - `bytes`: The number of bytes that represent the sensor's data in the transmission.
 */
const SensorTypes = {
    DIG_IN: { type: 0, precision: 1, signed: false, bytes: 1 },
    DIG_OUT: { type: 1, precision: 1, signed: false, bytes: 1 },
    ANL_IN: { type: 2, precision: 100, signed: true, bytes: 2 },
    ANL_OUT: { type: 3, precision: 100, signed: true, bytes: 2 },
    ILLUM_SENS: { type: 101, precision: 1, signed: false, bytes: 2 },
    PRSNC_SENS: { type: 102, precision: 1, signed: false, bytes: 1 },
    TEMP_SENS: { type: 103, precision: 10, signed: true, bytes: 2 },
    HUM_SENS: { type: 104, precision: 2, signed: false, bytes: 1 },
    ACCRM_SENS: { type: 113, precision: 1000, signed: true, bytes: 6 },
    BARO_SENS: { type: 115, precision: 10, signed: false, bytes: 2 },
    GYRO_SENS: { type: 134, precision: 100, signed: true, bytes: 6 },
    GPS_LOC: { type: 136, precision: 10000, signed: true, bytes: 12 }
};

/**
* @SensorTypes Reference Table:
*
* | Sensor Name | LPP  | IPSO   | Decimal Precision  | Signed | Size Bytes   |
* |-------------|------|--------|--------------------|--------|--------------|
* | DIG_IN      | 0    | 3200   |  1                 | false  | 1            |
* | DIG_OUT     | 1    | 3201   |  1                 | false  | 1            |
* | ANL_IN      | 2    | 3202   |  100               | true   | 2            |
* | ANL_OUT     | 3    | 3203   |  100               | true   | 2            |
* | ILLUM_SENS  | 101  | 3301   |  1                 | false  | 2            |
* | PRSNC_SENS  | 102  | 3302   |  1                 | false  | 1            |
* | TEMP_SENS   | 103  | 3303   |  10                | true   | 2            |
* | HUM_SENS    | 104  | 3304   |  10                | false  | 1            |
* | ACCRM_SENS  | 113  | 3313   |  1000              | true   | 6            |
* | BARO_SENS   | 115  | 3315   |  10                | false  | 2            |
* | GYRO_SENS   | 134  | 3334   |  100               | true   | 6            |
* | GPS_LOC     | 136  | 3336   |  10000             | true   | 12           |
*/

/**
 * @brief Decodes the uplink data payload based on the specified payload version.
 *
 * This function decodes the input payload by examining the payload version. For encoding version 1.
 *
 * @param input A structure containing the payload to be decoded.
 * @return Returns an object containing the decoded data, the version of the coding used, 
 *         and arrays for warnings and errors. The returned object has the following structure:
 *         {
 *           decoder_version: <version>,    // Integer representing the payload version
 *           data: <decoded_data>,          // Object containing the decoded payload
 *           warnings: <warnings_array>,    // Array of strings representing any warnings
 *           errors: <errors_array>,        // Array of strings representing any errors encountered
 *         }
 */
function decodeUplink(input) {
    let bytes = input.bytes;
    let payload_version = input.fPort;
    let decoded = {};

    if (payload_version == 1) { 
        decoded = processPayloadVersion_ONE(bytes, decoded);
    } else {
        decoded.errors = ["Payload Version not supported: " + payload_version];
    }

    return {
        decoder_version: payload_version,
        data: decoded,
        warnings: [],
        errors: [],
    };
}

/**
 * @brief Decodes a value from a byte array based on the specified parameters.
 *
 * This function extracts a value starting from index `i` in the `bytes` array, interpreting
 * a sequence of `byteLength` bytes according to whether the value is signed or unsigned,
 * and then adjusting it by a given `precision`.
 *
 * @param bytes The array of bytes from which the value is to be extracted.
 * @param i The starting index in the `bytes` array from which to begin decoding the value.
 * @param isSigned A boolean indicating whether the value to be decoded is signed (true) or unsigned (false).
 * @param precision The factor by which the raw decoded value should be divided to obtain the final value.
 *                  This allows for the representation of fractional values without using floating point numbers
 *                  in the encoded data.
 * @param byteLength The number of bytes that make up the value to be decoded.
 *
 * @return An object containing two properties: `value` and `index`. `value` is the decoded number adjusted
 *         by the `precision`, and `index` is the new index in the `bytes` array after decoding the value,
 *         which can be used for subsequent decoding operations.
 */
function decodeValue(bytes, i, isSigned, precision, byteLength) {
    let value = 0;
    for (let byteIndex = 0; byteIndex < byteLength; byteIndex++) {
        value = (value << 8) | bytes[i+byteIndex];
    }

    i += byteLength;

    if (isSigned && (value & (1 << (8 * byteLength - 1)))) {
        value = value - (1 << (8 * byteLength));
    }
    return { value: value / precision, index: i };
}

/**
 * @brief Processes and decodes payload version 1.
 *
 * This function iterates through the bytes of the payload, decoding each segment according to the sensor type
 * it represents. The decoded values are then added to the `decoded` object with keys
 * representing the sensor type and channel.
 *
 * @param bytes An array of bytes representing the payload to be decoded.
 * @param decoded An initially empty object that will be populated with the decoded sensor values.
 * @return Returns the `decoded` object populated with keys and values representing the decoded sensor data.
 *     
 */
function processPayloadVersion_ONE(bytes, decoded) {
    for (let i = 0; i < bytes.length;) {
        let channel = bytes[i++];
        let type = bytes[i++];
        let decodeResult;

        switch (type) {
            case SensorTypes.DIG_IN.type:
                decodeResult = decodeValue(bytes, i, SensorTypes.DIG_IN.signed,
                    SensorTypes.DIG_IN.precision, SensorTypes.DIG_IN.bytes);
                i = decodeResult.index;
                decoded['digital_' + channel] = decodeResult.value;
                break;
            case SensorTypes.DIG_OUT.type:
                decodeResult = decodeValue(bytes, i, SensorTypes.DIG_OUT.signed,
                    SensorTypes.DIG_OUT.precision, SensorTypes.DIG_OUT.bytes);
                i = decodeResult.index; // Update index
                decoded['digital_' + channel] = decodeResult.value;
                break;
            case SensorTypes.ANL_IN.type:
                decodeResult = decodeValue(bytes, i, SensorTypes.ANL_IN.signed,
                    SensorTypes.ANL_IN.precision, SensorTypes.ANL_IN.bytes);
                i = decodeResult.index;
                decoded['analog_' + channel] = decodeResult.value;
                break;
            case SensorTypes.ANL_OUT.type:
                decodeResult = decodeValue(bytes, i, SensorTypes.ANL_OUT.signed,
                    SensorTypes.ANL_OUT.precision, SensorTypes.ANL_OUT.bytes);
                i = decodeResult.index;
                decoded['analog_' + channel] = decodeResult.value;
                break;
            case SensorTypes.ILLUM_SENS.type:
                decodeResult = decodeValue(bytes, i, SensorTypes.ILLUM_SENS.signed,
                    SensorTypes.ILLUM_SENS.precision, SensorTypes.ILLUM_SENS.bytes);
                i = decodeResult.index;
                decoded['illumination_' + channel] = decodeResult.value;
                break;
            case SensorTypes.PRSNC_SENS.type:
                decodeResult = decodeValue(bytes, i, SensorTypes.PRSNC_SENS.signed,
                    SensorTypes.PRSNC_SENS.precision, SensorTypes.PRSNC_SENS.bytes);
                i = decodeResult.index;
                decoded['presence_' + channel] = decodeResult.value;
                break;
            case SensorTypes.TEMP_SENS.type:
                decodeResult = decodeValue(bytes, i, SensorTypes.TEMP_SENS.signed,
                    SensorTypes.TEMP_SENS.precision, SensorTypes.TEMP_SENS.bytes);
                i = decodeResult.index;
                decoded['temperature_' + channel] = decodeResult.value;
                break;
            case SensorTypes.HUM_SENS.type:
                decodeResult = decodeValue(bytes, i, SensorTypes.HUM_SENS.signed,
                    SensorTypes.HUM_SENS.precision, SensorTypes.HUM_SENS.bytes);
                i = decodeResult.index;
                decoded['humidity_' + channel] = decodeResult.value;
                break;
            case SensorTypes.ACCRM_SENS.type:
                let accDecodeX = decodeValue(bytes, i, SensorTypes.ACCRM_SENS.signed, SensorTypes.ACCRM_SENS.precision, SensorTypes.ACCRM_SENS.bytes / 3);
                i = accDecodeX.index;
                let accDecodeY = decodeValue(bytes, i, SensorTypes.ACCRM_SENS.signed, SensorTypes.ACCRM_SENS.precision, SensorTypes.ACCRM_SENS.bytes / 3);
                i = accDecodeY.index;
                let accDecodeZ = decodeValue(bytes, i, SensorTypes.ACCRM_SENS.signed, SensorTypes.ACCRM_SENS.precision, SensorTypes.ACCRM_SENS.bytes / 3);
                i = accDecodeZ.index;
                decoded['accelerometer_' + channel] = {
                    x: accDecodeX.value,
                    y: accDecodeY.value,
                    z: accDecodeZ.value
                };
                break;
            case SensorTypes.BARO_SENS.type:
                let baroDecode = decodeValue(bytes, i, SensorTypes.BARO_SENS.signed, SensorTypes.BARO_SENS.precision, SensorTypes.BARO_SENS.bytes);
                i = baroDecode.index;
                decoded['barometer_' + channel] = baroDecode.value;
                break;

            case SensorTypes.GYRO_SENS.type:
                let gyroDecodeX = decodeValue(bytes, i, SensorTypes.GYRO_SENS.signed, SensorTypes.GYRO_SENS.precision, SensorTypes.GYRO_SENS.bytes / 3);
                i = gyroDecodeX.index;
                let gyroDecodeY = decodeValue(bytes, i, SensorTypes.GYRO_SENS.signed, SensorTypes.GYRO_SENS.precision, SensorTypes.GYRO_SENS.bytes / 3);
                i = gyroDecodeY.index;
                let gyroDecodeZ = decodeValue(bytes, i, SensorTypes.GYRO_SENS.signed, SensorTypes.GYRO_SENS.precision, SensorTypes.GYRO_SENS.bytes / 3);
                i = gyroDecodeZ.index;
                decoded['gyroscope_' + channel] = {
                    x: gyroDecodeX.value,
                    y: gyroDecodeY.value,
                    z: gyroDecodeZ.value
                };
                break;
            case SensorTypes.GPS_LOC.type:
                let gpsDecodeX = decodeValue(bytes, i, SensorTypes.GPS_LOC.signed, SensorTypes.GPS_LOC.precision, SensorTypes.GPS_LOC.bytes / 3);
                i = gpsDecodeX.index;
                let gpsDecodeY = decodeValue(bytes, i, SensorTypes.GPS_LOC.signed, SensorTypes.GPS_LOC.precision, SensorTypes.GPS_LOC.bytes / 3);
                i = gpsDecodeY.index;
                let gpsDecodeZ = decodeValue(bytes, i, SensorTypes.GPS_LOC.signed, SensorTypes.GPS_LOC.precision / 100, SensorTypes.GPS_LOC.bytes / 3);
                i = gpsDecodeZ.index;
                decoded['gps_' + channel] = {
                    x: gpsDecodeX.value,
                    y: gpsDecodeY.value,
                    z: gpsDecodeZ.value
                };
                break;
            default: // Unknown data type
                decoded.errors = ["Unknown type: " + type];
                i = bytes.length; // Skip the rest of the payload
                break;
        }
    }
    return decoded;
}
