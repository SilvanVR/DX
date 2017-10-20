#pragma once

#include "Core/OS/FileSystem/file.h"

void TestFileStuff()
{
    OS::BinaryFile file;

    file.open("test.txt", OS::EFileMode::READ_WRITE_OVERWRITE);

    //Thread& t = file.readAsync( []( String data ) {
    //    LOG( data );
    //});
    // t.wait();

    if (file.exists())
    {
        LOG("File exists!\n");

        file.clear();

        //file.write("%s= %d\n", "Value", 42);
        //file.write("%s= %d\n", "Value2", 13);

        //char str[10];
        //int number;
        //file.read("%s %d", str, &number);

        //char str2[10];
        //int number2;
        //file.read("%s %d", str2, &number2);

        //file.setReadCursor(2);
        //double num = file.nextDouble();
        //LOG( TS(num) );

        file.write("A\n");
        file.write("B\n");
        file.write("C\n");
        file.write("[General]\n");
        file.write("Value=");
        file.write(66);

        file.setWriteCursor(0);
        file.append("Z");

        LOG("----- Listing File-Contents: ----- ", Color::YELLOW);
        String fileContents = file.readAll();
        LOG(fileContents, Color::YELLOW);
        LOG("---------------------------------- ", Color::YELLOW);

        while (!file.eof())
        {
            String line = file.readLine();
            LOG(line);
        }

        Size fileSize = file.getFileSize();
        LOG("Bytes: " + TS(fileSize));

        file.setReadCursor(0);
        while (!file.eof())
        {
            U8 nextChar = file.nextChar();
            String c(1, nextChar);
            LOG( c );
        }

    }
    else
    {
        LOG("Creating new file...\n");

        file.write("");
    }
}
