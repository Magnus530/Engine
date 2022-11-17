#include "epch.h"
#include "ObjLoader.h"

#include <iostream>
#include <fstream>

namespace Engine {
    static ObjLoader* m_Instance{ nullptr };

	//ObjLoader::ObjLoader(std::string filename, std::vector<glm::vec3>& vertices, std::vector<unsigned int>& indices)
	//{
	//	ReadFile(filename, vertices, indices);
	//}
	
    ObjLoader::ObjLoader()
    {}
	ObjLoader::~ObjLoader()
	{

	}
    ObjLoader* ObjLoader::Get()
    {
        if (!m_Instance) { m_Instance = new ObjLoader(); }
        return m_Instance;
    }

	void ObjLoader::ReadFile(std::string filename, std::vector<glm::vec3>& vertices, std::vector<uint32_t>& indices)
	{
        /* Assumes a default path */
		std::string path = "assets/meshes/";
		std::string objType = ".obj";
		std::string file = path + filename + objType;

		std::ifstream fileIn;
		fileIn.open(file, std::ifstream::in);
		if (!fileIn)
            E_CORE_ERROR("Could not open file: {0} : {1}", filename, file);
        else
            E_CORE_TRACE("Successfully read file: {0} : {1}", filename, file);


        std::string oneLine;
        std::string oneWord;

        std::vector<glm::vec3> tempVertices;
        std::vector<glm::vec3> tempNormals;
        std::vector<glm::vec2> tempUVs;

        //GLuint temp_index = 0;
        unsigned int temp_index = 0;

        while (std::getline(fileIn, oneLine))
        {
            std::stringstream sStream;
            sStream << oneLine;

            oneWord = "";
            sStream >> oneWord;

            if (oneWord == "#"){
                // Ignore
                continue;
            }
            if (oneWord == ""){
                // Ignore
                continue;
            }
            if (oneWord == "mtllib"){
                // Ignore
                continue;
            }
            if (oneWord == "o"){
                // Ignore
                continue;
            }
            if (oneWord == "usemtl"){
                // Ignore
                continue;
            }
            if (oneWord == "s"){
                // Ignore
                continue;
            }

            if (oneWord == "v"){
                //QVector3D tempVertex;
                glm::vec3 tempVertexLocation;
                sStream >> oneWord;
                tempVertexLocation.x = std::stof(oneWord);
                //tempVertex.set''X(std::stof(oneWord));
                sStream >> oneWord;
                tempVertexLocation.y = std::stof(oneWord);
                //tempVertex.setY(std::stof(oneWord));
                sStream >> oneWord;
                tempVertexLocation.z = std::stof(oneWord);
                //tempVertex.setZ(std::stof(oneWord));

                tempVertices.push_back(tempVertexLocation);

                continue;
            }
            if (oneWord == "vt"){
                //QVector2D tempUV;
                glm::vec2 tempUV;
                sStream >> oneWord;
                tempUV.x = std::stof(oneWord);
                //tempUV.setX(std::stof(oneWord));
                sStream >> oneWord;
                tempUV.y = std::stof(oneWord);
                //tempUV.setY(std::stof(oneWord));

                tempUVs.push_back(tempUV);

                continue;
            }
            if (oneWord == "vn"){
                //QVector3D tempNormal;
                glm::vec3 tempNormal;
                sStream >> oneWord;
                tempNormal.x = std::stof(oneWord);
                sStream >> oneWord;
                tempNormal.y = std::stof(oneWord);
                //tempNormal.setY(std::stof(oneWord));
                sStream >> oneWord;
                tempNormal.z = std::stof(oneWord);
                //tempNormal.setZ(std::stof(oneWord));

                tempNormals.push_back(tempNormal);

                continue;
            }
            if (oneWord == "f")
            {
                int index{ 0 }, normal{ 0 }, uv{ 0 };
                for (int i = 0; i < 3; i++)
                {
                    sStream >> oneWord;
                    std::stringstream tempWord(oneWord);
                    std::string segment;
                    std::vector<std::string> segmentArray;
                    while (std::getline(tempWord, segment, '/'))
                    {
                        segmentArray.push_back(segment);
                    }
                    index = std::stoi(segmentArray[0]);
                    if (segmentArray[1] != "")
                        uv = std::stoi(segmentArray[1]);
                    normal = std::stoi(segmentArray[2]);

                    --index;
                    --uv;
                    --normal;

                    
                    // MÅ FORANDRES PÅ
                    vertices.push_back(tempVertices[index]);
                    indices.push_back(temp_index++);
                    /* Lag vertex */
                    //Vertex tempVert{ tempVertices[index], tempNormals[normal], tempUVs[uv] };
                    //mVertices.push_back(tempVert);

                    /* Lag indekser */
                    //mIndices.push_back(temp_index++);
                }
                continue;
            }
        }
        
        fileIn.close();
	}
}