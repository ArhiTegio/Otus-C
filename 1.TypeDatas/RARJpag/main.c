#define NUL '\0'
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>


#pragma pack(push, 1)
typedef struct _tagEOCD
{
    uint16_t diskNumber;
    uint16_t startDiskNumber;
    uint16_t numberCentralDirectoryRecord;
    uint16_t totalCentralDirectoryRecord;
    uint32_t sizeOfCentralDirectory;
    uint32_t centralDirectoryOffset;
    uint16_t commentLength;
    //const uint8_t *comment;   /* Archive comment. */
} EOCD;
#pragma pack(pop)


typedef struct _tagCentralDirectoryFileHeader
{
    uint32_t signature;
    uint16_t versionMadeBy;
    //uint16_t versionToExtract;
    uint16_t generalPurposeBitFlag;
    uint16_t compressionMethod;
    uint16_t modificationTime;
    uint16_t modificationDate;
    uint32_t crc32;
    uint32_t compressedSize;
    uint16_t uncompressedSize;
    uint16_t filenameLength;
    uint16_t extraFieldLength;
    uint16_t extraFieldLength2;
    char *filename;
    char *extraFild;
} CentralDirectoryFileHeader;

typedef struct { char* c; char b; } a;


char* to_string(int number)
{
    int length = snprintf( NULL, 0, "%d", number );
    char* str = malloc( length + 1 );
    snprintf( str, length + 1, "%d", number );
    return str;
    free(str);
}

int found_eocd(int fileSize, FILE *stream, EOCD *eodc)
{
    size_t pos = -1;
    int signature;
    for (size_t offset = fileSize - sizeof(EOCD); offset > 0; --offset)
    {
        fseek(stream, offset, SEEK_SET);
        fread(&signature, sizeof(int), 1,stream);
        if (0x06054b50 == signature)
        {
            pos = offset;
            // Есть контакт!
            fread(&eodc->diskNumber, sizeof(uint16_t), 1,stream);
            fread(&eodc->startDiskNumber, sizeof(uint16_t), 1,stream);
            fread(&eodc->numberCentralDirectoryRecord, sizeof(uint16_t), 1,stream);
            fread(&eodc->totalCentralDirectoryRecord, sizeof(uint16_t), 1,stream);
            fread(&eodc->sizeOfCentralDirectory, sizeof(uint32_t), 1,stream);
            fread(&eodc->centralDirectoryOffset, sizeof(uint32_t), 1,stream);
            fread(&eodc->commentLength, sizeof(uint16_t), 1,stream);
            break;
        }
    }
    return pos;
}

void print_array_char(char *arr, int len)
{
    for(int j = 0; j < len; ++j)
    {
        printf("%c", arr[j]);
    }
}

int get_cdfh(int fileSize, FILE *stream, CentralDirectoryFileHeader *cdfh)
{
    size_t pos = -1;
    int signature;

    for (size_t offset = fileSize - sizeof(int); offset >= 0; --offset)
    {
        fseek(stream, offset, SEEK_SET);
        fread(&signature, sizeof(int), 1,stream);
        if (0x04034b50 == signature)
        {
            pos = offset;
            // Есть контакт!
            fseek(stream, offset, SEEK_SET);
            fread(&cdfh->signature, sizeof(uint32_t), 1,stream);
            fread(&cdfh->versionMadeBy, sizeof(uint16_t), 1,stream);
            fread(&cdfh->generalPurposeBitFlag, sizeof(uint16_t), 1,stream);
            fread(&cdfh->compressionMethod, sizeof(uint16_t), 1,stream);
            fread(&cdfh->modificationTime, sizeof(uint16_t), 1,stream);
            fread(&cdfh->modificationDate, sizeof(uint16_t), 1,stream);
            fread(&cdfh->crc32, sizeof(uint16_t), 1,stream);
            fread(&cdfh->compressedSize, sizeof(uint32_t), 1,stream);
            fread(&cdfh->uncompressedSize, sizeof(uint32_t), 1,stream);
            fread(&cdfh->extraFieldLength, sizeof(uint16_t), 1,stream);
            fread(&cdfh->filenameLength, sizeof(uint16_t), 1,stream);

            if(cdfh->compressionMethod == 0 || cdfh->compressionMethod == 8)
            {
                printf("------------------------------\n");
                printf("signature %X\n", cdfh->signature);
                printf("versionMadeBy %d\n", cdfh->versionMadeBy);
                //printf("versionToExtract %d\n", cdfh->versionToExtract);
                printf("generalPurposeBitFlag %d\n", cdfh->generalPurposeBitFlag);
                printf("compressionMethod %d\n", cdfh->compressionMethod);
                printf("modificationTime %d\n", cdfh->modificationTime);
                printf("modificationDate %d\n", cdfh->modificationDate);
                printf("crc32 %d\n", cdfh->crc32);
                printf("compressedSize %d\n", cdfh->compressedSize);
                printf("uncompressedSize %d\n", cdfh->uncompressedSize);
                printf("filenameLength %d\n", cdfh->filenameLength);
                printf("extraFieldLength %d\n", cdfh->extraFieldLength);

                fseek(stream, ftell(stream)+1, 0);
                if(cdfh->filenameLength > 0)
                {
                    char *str[cdfh->filenameLength+2];
                    cdfh->filename = str;
                    fgets(cdfh->filename, cdfh->filenameLength+2, stream);
                    printf("filename: ");
                    print_array_char(cdfh->filename, cdfh->filenameLength+2);
                    printf("\n");
                }

                if(cdfh->extraFieldLength > 0)
                {
                    char *str[cdfh->extraFieldLength+2];
                    cdfh->extraFild = str;
                    fgets(cdfh->extraFild, cdfh->extraFieldLength+2, stream);
                    printf("extraFild: ");
                    print_array_char(cdfh->extraFild, cdfh->extraFieldLength+2);
                    printf("\n");
                }
            }
        }
    }
    return pos;
}


int main(int argc, char **argv)
{
    if(argc > 0)
    {
        printf("Name file: %s\n",argv[1]);
        int size_file = 0;
        FILE *file;
        file = fopen(argv[1], "rb");
        int idx = getc(file);
        char snum[5];
        while (idx != EOF)
        {
            idx = getc(file);
            size_file++;
        }


        printf("File size: %s\n", to_string(size_file));
        printf("sizeof(EOCD) == %d\n", sizeof(EOCD));

        EOCD eodc = {0,0,0,0,0,0,0};
        int pos_eodc = found_eocd(size_file, file, &eodc);
        printf("EOCD signature %d\n", pos_eodc);
        if(pos_eodc != -1)
        {
            CentralDirectoryFileHeader cdfh = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
            printf("centralDirectoryOffset %d\n", eodc.centralDirectoryOffset);
            printf("commentLength %d\n", eodc.commentLength);
            printf("diskNumber %d\n", eodc.diskNumber);
            printf("numberCentralDirectoryRecord %d\n", eodc.numberCentralDirectoryRecord);
            printf("sizeOfCentralDirectory %d\n", eodc.sizeOfCentralDirectory);
            printf("startDiskNumber %d\n", eodc.startDiskNumber);
            printf("totalCentralDirectoryRecord %d\n", eodc.totalCentralDirectoryRecord);


            printf("CentralDirectoryFileHeader size = %d\n", sizeof(CentralDirectoryFileHeader));
            get_cdfh(size_file, file, &cdfh);
        }
        else
        {
            printf("Архив не найден.");
        }
        fclose(file);
    }
    else
    {
        printf("Test 2\n");
    }

    return 0;
}



