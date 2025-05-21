// LittleFsCOM.hpp
#pragma once
#include "ComModule.hpp"
#include <LittleFS.h>


class FileTransferState {
    public:
        String filename;
        uint32_t fileSize = 0;
        uint32_t totalChunks = 0;
        uint32_t currentChunk = 0;
        bool isActive = false;
    
        void reset() {
            filename = "";
            fileSize = 0;
            totalChunks = 0;
            currentChunk = 0;
            isActive = false;
        }
    };

#define COM_FILE_P1 par0
#define COM_FILE_P2 par1
#define COM_FILE_P3 par2
#define COM_FILE_P4 par3

#define COM_FILE_INIT   0x00
#define COM_FILE_DATA   0x0D
#define MAX_FILE_CHUNK_SIZE 128 // Maximum file chunk size for splitting.




class LittleFsCOM : public ComModule {
public:
    LittleFsCOM() : ComModule('F') {LittleFS.begin();}

    bool dispatchFrame(ComFrame* pFrame) override;
    
private:
    String _listDirectory(String path, String header);
    bool _readFile(ComFrame *pFrame);
    bool _writeFile(ComFrame *pFrame);
    bool _deleteFile(ComFrame *pFrame);
    bool _createDirectory(ComFrame *pFrame);
    bool _deleteDirectory(ComFrame *pFrame);

    FileTransferState _fileTransferState;
};
