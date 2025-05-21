#include "LittleFsCOM.hpp"
#include <Base64.hpp>





bool LittleFsCOM::dispatchFrame(ComFrame* pFrame) {
    if (pFrame->command == "FILE read") {
        return _readFile(pFrame);
    } else if (pFrame->command == "FILE write") {
        return _writeFile(pFrame);
    } else if (pFrame->command == "FILE delete") {
        return _deleteFile(pFrame);
    } else if (pFrame->command == "FILE mkdir") {
        return _createDirectory(pFrame);
    } else if (pFrame->command == "FILE rmdir") {
        return _deleteDirectory(pFrame);
    }  else if (pFrame->command == "FILE list"){
        pFrame->res = "directory of LittleFS:\n";
        pFrame->res += _listDirectory(String("/"),String("")); 
        return true;
    }    

    return false;
        pFrame->res = "Error: Unknown LittleFS command.";
    return false;
}

String LittleFsCOM::_listDirectory(String path, String header) {
    String result = header;
    Dir dir = LittleFS.openDir(path);

    while (dir.next()) {
        if (dir.isDirectory()) {
            result += path + dir.fileName() + "/\n";
            result += _listDirectory(path + dir.fileName() + "/", String("")); // Recursive call for subdirectories
        } else {
            result += path + dir.fileName() + "*" + String(dir.fileSize()) + "\n";
        }
    }

    return result;
}

bool LittleFsCOM::_readFile(ComFrame *pFrame) {
    uint32_t sequenz = pFrame->cfg.COM_FILE_P1.uint32;

    if (sequenz == COM_FILE_INIT) {
        _fileTransferState.reset();

        _fileTransferState.filename = pFrame->cfg.str;
        File file = LittleFS.open(_fileTransferState.filename, "r");
        if (!file) {
            pFrame->res = "Error: File not found: " + _fileTransferState.filename;
            return false;
        }

        _fileTransferState.fileSize = file.size();
        _fileTransferState.totalChunks = (_fileTransferState.fileSize + MAX_FILE_CHUNK_SIZE - 1) / MAX_FILE_CHUNK_SIZE;
        _fileTransferState.currentChunk = 1;
        _fileTransferState.isActive = true;

        pFrame->cfg.COM_FILE_P1.uint32 = COM_FILE_INIT;
        pFrame->cfg.COM_FILE_P2.uint32 = 0;
        pFrame->cfg.COM_FILE_P3.uint32 = _fileTransferState.totalChunks;
        pFrame->cfg.COM_FILE_P4.uint32 = _fileTransferState.fileSize;
        pFrame->cfg.str = _fileTransferState.filename;

        pFrame->res = "";
        file.close();
        return true;
    } else if (sequenz == COM_FILE_DATA) {
        if (!_fileTransferState.isActive) {
            pFrame->res = "Error: No active file read sequence.";
            return false;
        }

        uint32_t chunk = pFrame->cfg.COM_FILE_P2.uint32;
        if ((chunk != _fileTransferState.currentChunk) || (chunk < 1)) {
            pFrame->res = "Error: Invalid chunk order.";
            return false;
        }

        File file = LittleFS.open(_fileTransferState.filename, "r");
        if (!file) {
            pFrame->res = "Error: File not found: " + _fileTransferState.filename;
            return false;
        }
        uint32_t offset = (_fileTransferState.currentChunk - 1) * MAX_FILE_CHUNK_SIZE;
        file.seek(offset);
        uint8_t buffer[MAX_FILE_CHUNK_SIZE];
        uint32_t bytesToRead = min((uint32_t)MAX_FILE_CHUNK_SIZE, _fileTransferState.fileSize - offset);
        file.read(buffer, bytesToRead);
        file.close();

        char bufferBase64[COM_FRAME_MAX_STR_LENGTH];
        encode_base64(buffer, bytesToRead, (uint8_t *)bufferBase64);
        pFrame->cfg.COM_FILE_P1.uint32 = COM_FILE_DATA;
        pFrame->cfg.COM_FILE_P2.uint32 = _fileTransferState.currentChunk;
        pFrame->cfg.COM_FILE_P3.uint32 = _fileTransferState.totalChunks;
        pFrame->cfg.COM_FILE_P4.uint32 = _fileTransferState.fileSize;
        pFrame->cfg.str = String(bufferBase64);
        pFrame->res = "";

        _fileTransferState.currentChunk++;
        if (_fileTransferState.currentChunk > _fileTransferState.totalChunks) {
            _fileTransferState.reset();
        }

        return true;
    }

    pFrame->res = "Error: Invalid sequence id P1:" + String(sequenz, HEX);
    _fileTransferState.reset();
    return false;
}

bool LittleFsCOM::_writeFile(ComFrame *pFrame) {
    uint32_t sequenz = pFrame->cfg.COM_FILE_P1.uint32;

    if (sequenz == COM_FILE_INIT) {
        _fileTransferState.reset();
        _fileTransferState.filename = pFrame->cfg.str;
        _fileTransferState.currentChunk = pFrame->cfg.COM_FILE_P2.uint32;
        _fileTransferState.totalChunks = pFrame->cfg.COM_FILE_P3.uint32;
        _fileTransferState.fileSize = pFrame->cfg.COM_FILE_P4.uint32;
        _fileTransferState.isActive = true;
        pFrame->res = "";

        if (LittleFS.exists(_fileTransferState.filename)) {
            if (!LittleFS.remove(_fileTransferState.filename)) {
                pFrame->res = "Error: Failed to delete existing file with same filename.";
                return false;
            }
            pFrame->res = "Existing file deleted.";
        }

        File file = LittleFS.open(_fileTransferState.filename, "w");
        if (!file) {
            pFrame->res += "Error: Failed to create new file: " + _fileTransferState.filename;
            _fileTransferState.reset();
            return false;
        }

        file.close();
        pFrame->cfg.COM_FILE_P1.uint32 = COM_FILE_INIT;
        pFrame->cfg.COM_FILE_P2.uint32 = 0;
        pFrame->cfg.COM_FILE_P3.uint32 = _fileTransferState.totalChunks;
        pFrame->cfg.COM_FILE_P4.uint32 = _fileTransferState.fileSize;
        pFrame->cfg.str = _fileTransferState.filename;

        _fileTransferState.currentChunk = 1;

        return true;
    } else if (sequenz == COM_FILE_DATA) {
        if (!_fileTransferState.isActive) {
            pFrame->res = "Error: No active file write sequence.";
            return false;
        }

        uint32_t chunk = pFrame->cfg.COM_FILE_P2.uint32;
        if (chunk != _fileTransferState.currentChunk) {
            pFrame->res = "Error: Invalid chunk order.";
            return false;
        }

        String base64Data = pFrame->cfg.str;
        uint8_t buffer[MAX_FILE_CHUNK_SIZE];
        size_t decodedLength = decode_base64((const uint8_t *)base64Data.c_str(), base64Data.length(), buffer);
        if (decodedLength == 0) {
            pFrame->res = "Error: Base64 decode failed.";
            return false;
        }

        File file = LittleFS.open(_fileTransferState.filename, "a");
        if (!file) {
            pFrame->res = "Error: Failed to write to file: " + _fileTransferState.filename;
            _fileTransferState.reset();
            return false;
        }
        file.write(buffer, decodedLength);
        file.close();

        if (_fileTransferState.currentChunk == _fileTransferState.totalChunks) {
            _fileTransferState.reset();
            pFrame->res = "File write completed.";
        } else {
            _fileTransferState.currentChunk++;
        }
        return true;
    }

    pFrame->res = "Error: Invalid sequence id P1:" + String(sequenz, HEX);
    _fileTransferState.reset();
    return false;
}

bool LittleFsCOM::_deleteFile(ComFrame *pFrame) {
    String filePath = pFrame->cfg.str;

    if (!LittleFS.exists(filePath)) {
        pFrame->res = "Error: File does not exist.";
        return false;
    }

    if (LittleFS.remove(filePath)) {
        return true;
    }

    pFrame->res = "Error: Failed to delete file.";
    return false;
}

bool LittleFsCOM::_createDirectory(ComFrame *pFrame) {
    String path = pFrame->cfg.str;

    if (LittleFS.exists(path)) {
        pFrame->res = "Error: Path already exists.";
        return false;
    }

    // LittleFS does not support creating empty directories directly
    File file = LittleFS.open(path + "/.keep", "w");
    if (!file) {
        pFrame->res = "Error: Failed to create directory.";
        return false;
    }
    file.close();

    return true;
}

bool LittleFsCOM::_deleteDirectory(ComFrame *pFrame) {
    String path = pFrame->cfg.str;

    if (!LittleFS.exists(path)) {
        pFrame->res = "Error: Path does not exist.";
        return false;
    }

    Dir dir = LittleFS.openDir(path);
    while (dir.next()) {
        if (!LittleFS.remove(path + "/" + dir.fileName())) {
            pFrame->res = "Error: Failed to delete file: " + dir.fileName();
            return false;
        }
    }

    // Remove the directory itself
    if (!LittleFS.remove(path + "/.keep")) {
        pFrame->res = "Error: Failed to remove directory.";
        return false;
    }

    return true;
}
