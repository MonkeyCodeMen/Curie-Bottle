#pragma once
#include <Arduino.h>
#include <LittleFS.h>
#include <vector>
#include <string>

/**
 * @brief Base class for logging data to a file.
 *
 * This class provides the foundation for logging data in different formats (e.g., CSV, binary).
 * Derived classes must implement the `writeToFile` method to define how data is written to the file.
 *
 * Example usage:
 * ```cpp
 * // Create a derived class instance (e.g., CSVLogFile or BinaryLogFile)
 * CSVLogFile log("log.csv", 2, {"Timestamp", "Value1", "Value2"}, 100, true);
 *
 * // Add a new dataset
 * int32_t data[] = {42, 84};
 * log.addNewData(millis(), data);
 * ```
 */

 #define LOG_FILE_CSV_SEPARATOR ';'
class LogFile
{
protected:
    String _fileName;                        // Name of the log file
    size_t _dataSetSize;                     // Number of values in each dataset
    std::vector<String> _columnHeaders;      // Column headers for the dataset
    size_t _maxSize;                         // Maximum number of datasets
    bool _overwriteOldData;                  // Behavior when max size is reached
    std::vector<std::vector<int32_t>> _data; // In-memory data storage

    /**
     * @brief Write the dataset to the file.
     *
     * This function must be implemented by derived classes.
     */
    virtual void writeToFile() = 0;

    /**
     * @brief Ensure data safety by syncing to the file system.
     */
    void syncToFile()
    {
        if (!LittleFS.exists(_fileName))
        {
            File file = LittleFS.open(_fileName, "w");
            file.close();
        }
    }

public:
    /**
     * @brief Constructor for the LogFile class.
     *
     * @param fileName Name of the log file.
     * @param dataSetSize Number of values in each dataset.
     * @param columnHeaders Column headers for the dataset.
     * @param maxSize Maximum number of datasets to store.
     * @param overwriteOldData If true, overwrite oldest data when max size is reached.
     */
    LogFile(const String &fileName, size_t dataSetSize, const std::vector<String> &columnHeaders, size_t maxSize, bool overwriteOldData)
        : _fileName(fileName), _dataSetSize(dataSetSize), _columnHeaders(columnHeaders), _maxSize(maxSize), _overwriteOldData(overwriteOldData)
    {
        LittleFS.begin();
        syncToFile();
    }

    virtual ~LogFile() = default;

    /**
     * @brief Add a new dataset to the log.
     *
     * @param now Current system time in milliseconds.
     * @param pNewDataSet Pointer to the new dataset.
     */
    void addNewData(uint32_t now, int32_t *pNewDataSet)
    {
        if (_dataSetSize == 0 || pNewDataSet == nullptr)
            return;

        // Create a new dataset with the timestamp
        std::vector<int32_t> newDataSet;
        newDataSet.push_back(now);
        for (size_t i = 0; i < _dataSetSize; i++)
        {
            newDataSet.push_back(pNewDataSet[i]);
        }

        // Handle size limit
        if (_data.size() >= _maxSize)
        {
            if (_overwriteOldData)
            {
                _data.erase(_data.begin()); // Remove the oldest dataset
            }
            else
            {
                return; // Ignore new data
            }
        }

        // Add the new dataset
        _data.push_back(newDataSet);

        // Write to file
        writeToFile();
    }
};

/**
 * @brief Class for logging data in CSV format.
 *
 * This class logs data in a human-readable CSV format. Each dataset is written as a row in the CSV file.
 *
 * Example usage:
 * ```cpp
 * // Create a CSV log file
 * CSVLogFile csvLog("log.csv", 2, {"Timestamp", "Value1", "Value2"}, 100, true);
 *
 * // Add a new dataset
 * int32_t data[] = {42, 84};
 * csvLog.addNewData(millis(), data);
 * ```
 */
class CSVLogFile : public LogFile
{
private:
    /**
     * @brief Write the dataset to the CSV file.
     */
    void writeToFile() override
    {
        File file = LittleFS.open(_fileName, "w");
        if (!file)
            return;

        // Write column headers
        for (size_t i = 0; i < _columnHeaders.size(); i++)
        {
            file.print(_columnHeaders[i]);
            if (i < _columnHeaders.size() - 1)
                file.print(LOG_FILE_CSV_SEPARATOR);
        }
        file.println();

        // Write data
        for (const auto &dataSet : _data)
        {
            for (size_t i = 0; i < dataSet.size(); i++)
            {
                file.print(dataSet[i]);
                if (i < dataSet.size() - 1)
                    file.print(LOG_FILE_CSV_SEPARATOR);
            }
            file.println();
        }

        file.close();
    }

public:
    CSVLogFile(const String &fileName, size_t dataSetSize, const std::vector<String> &columnHeaders, size_t maxSize, bool overwriteOldData)
        : LogFile(fileName, dataSetSize, columnHeaders, maxSize, overwriteOldData) {}
};

/**
 * @brief Class for logging data in binary format.
 *
 * This class logs data in a compact binary format. Each dataset is written as raw binary data to the file.
 *
 * Example usage:
 * ```cpp
 * // Create a binary log file
 * BinaryLogFile binLog("log.bin", 2, {"Timestamp", "Value1", "Value2"}, 100, false);
 *
 * // Add a new dataset
 * int32_t data[] = {128, 256};
 * binLog.addNewData(millis(), data);
 * ```
 */
class BinaryLogFile : public LogFile
{
private:
    /**
     * @brief Write the dataset to the binary file.
     */
    void writeToFile() override
    {
        File file = LittleFS.open(_fileName, "w");
        if (!file)
            return;

        // Write data
        for (const auto &dataSet : _data)
        {
            for (const auto &value : dataSet)
            {
                file.write((uint8_t *)&value, sizeof(value));
            }
        }

        file.close();
    }

public:
    BinaryLogFile(const String &fileName, size_t dataSetSize, const std::vector<String> &columnHeaders, size_t maxSize, bool overwriteOldData)
        : LogFile(fileName, dataSetSize, columnHeaders, maxSize, overwriteOldData) {}
};

/**
 * @brief Class for logging data with a buffer.
 *
 * This class logs data with a buffer and supports both CSV and binary formats.
 *
 * Example usage:
 * ```cpp
 * // Create a buffered log file
 * BufferedLogFile bufferedLog("log.csv", 2, {"Timestamp", "Value1", "Value2"}, 100, true, BufferedLogFile::CSV);
 *
 * // Add a new dataset
 * int32_t data[] = {42, 84};
 * bufferedLog.addNewData(millis(), data);
 * ```
 */
class BufferedLogFile : public LogFile
{
public:
    enum WriteMode
    {
        CSV,
        BINARY
    };

private:
    size_t _bufferSize;     // Maximum number of datasets in the buffer
    bool _overwriteOldData; // Behavior when buffer is full
    WriteMode _writeMode;   // Write mode (CSV or binary)
public:
    /**
     * @brief Constructor for the BufferedLogFile class.
     *
     * @param fileName Name of the log file.
     * @param dataSetSize Number of values in each dataset.
     * @param columnHeaders Column headers for the dataset.
     * @param bufferSize Maximum number of datasets in the buffer.
     * @param overwriteOldData If true, overwrite oldest data when buffer is full.
     * @param writeMode Write mode (CSV or binary).
     */
    BufferedLogFile(const String &fileName, size_t dataSetSize, const std::vector<String> &columnHeaders, size_t bufferSize, bool overwriteOldData, WriteMode writeMode)
        : LogFile(fileName, dataSetSize, columnHeaders, bufferSize, overwriteOldData), _bufferSize(bufferSize), _overwriteOldData(overwriteOldData), _writeMode(writeMode) {}

    /**
     * @brief Write the buffered data to the file system.
     */
    void writeToFile() override
    {
        if (_writeMode == CSV)
        {
            writeToCSV();
        }
        else if (_writeMode == BINARY)
        {
            writeToBinary();
        }
    }

private:
    /**
     * @brief Write the buffered data to a CSV file.
     */
    void writeToCSV()
    {
        File file = LittleFS.open(_fileName, "w");
        if (!file)
            return;

        // Write column headers
        for (size_t i = 0; i < _columnHeaders.size(); i++)
        {
            file.print(_columnHeaders[i]);
            if (i < _columnHeaders.size() - 1)
                file.print(LOG_FILE_CSV_SEPARATOR);
        }
        file.println();

        // Write data
        for (const auto &dataSet : _data)
        {
            for (size_t i = 0; i < dataSet.size(); i++)
            {
                file.print(dataSet[i]);
                if (i < dataSet.size() - 1)
                    file.print(LOG_FILE_CSV_SEPARATOR);
            }
            file.println();
        }

        file.close();
    }

    /**
     * @brief Write the buffered data to a binary file.
     */
    void writeToBinary()
    {
        File file = LittleFS.open(_fileName, "w");
        if (!file)
            return;

        // Write data
        for (const auto &dataSet : _data)
        {
            for (const auto &value : dataSet)
            {
                file.write((uint8_t *)&value, sizeof(value));
            }
        }

        file.close();
    }
};
