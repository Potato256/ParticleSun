#ifndef VBO_H
#define VBO_H

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class vboReader
{
public:
	int vertexNum;
    float buf[24000] = {};
	vboReader(std::string path)
	{
        std::ifstream vboFile;
        std::stringstream vboStream;
        // ensure ifstream objects can throw exceptions:
        vboFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        try
        {
            // open files
            vboFile.open(path);
            // read file's buffer contents into streams
            vboStream << vboFile.rdbuf();
            // close file handlers
            vboFile.close();
        }
        catch (std::ifstream::failure& e)
        {
            std::cout << "ERROR::VBO_READ::FILE_NOT_SUCCESFULLY_READ: " << e.what() << std::endl;
        }
        vertexNum = 0;
        while (!vboStream.eof())
        {
            for (int i = 0; i < 8; ++i)
                vboStream >> buf[vertexNum * 8 + i]  ;
            ++vertexNum;
        }
        --vertexNum;
    }
};
#endif