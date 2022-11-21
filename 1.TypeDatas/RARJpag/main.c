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
            fread(eodc, sizeof(EOCD), 1, stream);
            break;
        }
    }
    return pos;
}


//Вывод строки в hex представлении
// Usage:
//     hexDump(desc, addr, len, perLine);
//         desc:    if non-NULL, printed as a description before hex dump.
//         addr:    the address to start dumping from.
//         len:     the number of bytes to dump.
//         perLine: number of bytes on each output line.
void hexDump (
    const char * desc,
    const void * addr,
    const int len,
    int perLine
) {
    // Silently ignore silly per-line values.
    if (perLine < 4 || perLine > 64) perLine = 16;
    int i;
    unsigned char buff[perLine+1];
    const unsigned char * pc = (const unsigned char *)addr;
    // Output description if given.
    if (desc != NULL) printf ("%s:\n", desc);
    // Length checks.
    if (len == 0) {
        printf("  ZERO LENGTH\n");
        return;
    }
    if (len < 0) {
        printf("  NEGATIVE LENGTH: %d\n", len);
        return;
    }
    // Process every byte in the data.
    for (i = 0; i < len; i++) {
        // Multiple of perLine means new or first line (with line offset).
        if ((i % perLine) == 0) {
            // Only print previous-line ASCII buffer for lines beyond first.
            if (i != 0) printf ("  %s\n", buff);
            // Output the offset of current line.
            printf ("  %04x ", i);
        }
        // Now the hex code for the specific character.
        printf (" %02x", pc[i]);
        // And buffer a printable ASCII character for later.
        if ((pc[i] < 0x20) || (pc[i] > 0x7e)) // isprint() may be better.
            buff[i % perLine] = '.';
        else
            buff[i % perLine] = pc[i];
        buff[(i % perLine) + 1] = '\0';
    }
    // Pad out last line if not exactly perLine characters.
    while ((i % perLine) != 0) {
        printf ("   ");
        i++;
    }
    // And print the final ASCII buffer.
    printf ("  %s\n", buff);
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
                //Для вывода дополнительных параметров у найденных файлов
                //printf("------------------------------\n");
                //printf("signature %X\n", cdfh->signature);
                //printf("versionMadeBy %d\n", cdfh->versionMadeBy);
                ////printf("versionToExtract %d\n", cdfh->versionToExtract);
                //printf("generalPurposeBitFlag %d\n", cdfh->generalPurposeBitFlag);
                //printf("compressionMethod %d\n", cdfh->compressionMethod);
                //printf("modificationTime %d\n", cdfh->modificationTime);
                //printf("modificationDate %d\n", cdfh->modificationDate);
                //printf("crc32 %d\n", cdfh->crc32);
                //printf("compressedSize %d\n", cdfh->compressedSize);
                //printf("uncompressedSize %d\n", cdfh->uncompressedSize);
                //printf("filenameLength %d\n", cdfh->filenameLength);
                //printf("extraFieldLength %d\n", cdfh->extraFieldLength);

                fseek(stream, ftell(stream)+1, 0);
                if(cdfh->filenameLength > 0)
                {
                    char *str_filename[cdfh->filenameLength+2];
                    cdfh->filename = str_filename;
                    fgets(cdfh->filename, cdfh->filenameLength+2, stream);

                    //Проверка hex вывода filename
                    //hexDump(cdfh->filename, &cdfh->filename, cdfh->filenameLength+1, 16);
                    //Не выводит название файла
                    //printf("%.*s\n", cdfh->filenameLength+1, cdfh->filename);

                    print_array_char(cdfh->filename, cdfh->filenameLength+1);
                    printf("\n");
                }

                if(cdfh->extraFieldLength > 0)
                {
                    char *str_extraFild[cdfh->extraFieldLength+2];
                    cdfh->extraFild = str_extraFild;
                    fgets(cdfh->extraFild, cdfh->extraFieldLength+2, stream);
                    //Для вывода extraFild файла
                    //print_array_char(cdfh->extraFild, cdfh->extraFieldLength+1);
                    //printf("\n");
                }
            }
        }
    }
    return pos;
}


int main(int argc, char **argv)
{
    if(argc == 2)
    {
        printf("Name file: %s\n",argv[1]);
        int size_file = 0;
        FILE *file;
        file = fopen(argv[1], "rb");
        if(file != NULL)
        {
            int idx = getc(file);
            while (idx != EOF)
            {
                idx = getc(file);
                size_file++;
            }


            printf("File size: %s\n", to_string(size_file));
            printf("sizeof(EOCD) == %ld\n", sizeof(EOCD));

            EOCD eodc = {0};
            int pos_eodc = found_eocd(size_file, file, &eodc);
            printf("EOCD signature %d\n", pos_eodc);
            if(pos_eodc != -1)
            {
                CentralDirectoryFileHeader cdfh = {0};
                printf("centralDirectoryOffset %d\n", eodc.centralDirectoryOffset);
                printf("commentLength %d\n", eodc.commentLength);
                printf("diskNumber %d\n", eodc.diskNumber);
                printf("numberCentralDirectoryRecord %d\n", eodc.numberCentralDirectoryRecord);
                printf("sizeOfCentralDirectory %d\n", eodc.sizeOfCentralDirectory);
                printf("startDiskNumber %d\n", eodc.startDiskNumber);
                printf("totalCentralDirectoryRecord %d\n", eodc.totalCentralDirectoryRecord);


                printf("CentralDirectoryFileHeader size = %ld\n", sizeof(CentralDirectoryFileHeader));
                get_cdfh(size_file, file, &cdfh);
            }
            else
            {
                printf("Архив не найден.");
            }
            fclose(file);
            free(file);
        }
        else{
            printf("Нет такого файла.");
        }
    }
    else
    {
        printf("Количество аргументов меньше или больше необходимого. Введите имя файла через пробел после имени функции.\n");
    }

    return 0;
}



