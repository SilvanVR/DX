#pragma once
/**********************************************************************
    class: ShaderParser (shader_parser.hpp)

    author: S. Hau
    date: March 1, 2018
**********************************************************************/

#include "Graphics/i_material.h"
#include "OS/FileSystem/file.h"
#include "Ext/JSON/json.hpp"
#include "locator.h"

using JSON = nlohmann::json;

namespace Core { namespace Assets {

    //**********************************************************************
    class MaterialParser
    {
    public:
        //----------------------------------------------------------------------
        // Tries to load a custom shader file format from the given file.
        // @Return:
        //  A new shader object if everything was successful. 
        // @Throws:
        //  std::runtime_error if something went wrong.
        //----------------------------------------------------------------------
        static MaterialPtr LoadMaterial( const OS::Path& filePath )
        {
            auto material = RESOURCES.createMaterial();
            UpdateMaterial( material, filePath );
            material->setName( filePath.getFileName() );
            return material;
        }

        //----------------------------------------------------------------------
        // Updates the given material from the given file.
        // @Throws:
        //  std::runtime_error if something went wrong.
        //----------------------------------------------------------------------
        static void UpdateMaterial( const MaterialPtr& material, const OS::Path& filePath )
        {
            OS::File file( filePath );

            // Parse json file
            JSON json;
            try {
                json = JSON::parse( file.readAll() );
            } catch (...) {
                throw std::runtime_error( "Failed to parse file as JSON. Please ensure that the file contains valid JSON." );
            }

            // First check if a shader is specified
            auto shader = json.find( "shader" );
            if ( shader != json.end() )
            {
                // Set shader. Will be loaded if not already loaded.
                String shaderPath = shader.value();
                material->setShader( ASSETS.getShader( shaderPath ) );
                json.erase( "shader" );
            }

            // Now parse all fields and set material data
            for (auto it = json.begin(); it != json.end(); it++)
            {
                String propName = it.key();

                // Retrieve data type for this property
                auto type = material->getDataType( propName.c_str() );
                switch (type)
                {
                case DataType::Int:
                {
                    try
                    {
                        material->setInt( propName.c_str(), it.value() );
                    } catch (...) { // Parsing was unsuccessful
                        LOG_WARN( "MaterialParser: Could not parse parameter '" + propName + "' as an int. Please ensure that it is a valid int." );
                    }
                    break;
                }
                case DataType::Float:
                {
                    try
                    {
                        material->setFloat( propName.c_str(), it.value() );
                    } catch (...) { // Parsing was unsuccessful
                        LOG_WARN( "MaterialParser: Could not parse parameter '" + propName + "' as an float. Please ensure that it is a valid float." );
                    }
                    break;
                }
                case DataType::Vec4: 
                {
                    try
                    {
                        // Vec4 are valid in hex-format or as object
                        if ( it.value().is_object() ) // i.e. {"x" : 1, "y" : 0, "z" : 0, "w" : 1} or {"r" : 1, "g" : 0, "b" : 0, "a" : 1}
                        {
                            material->setVec4( propName.c_str(), _ParseVec4( it.value() ) );
                        }
                        else if ( it.value().is_string() ) // i.e. "#FF00FF" or "#FF00FFFF"
                        {
                            String hex = it.value();
                            material->setColor( propName.c_str(), Color( hex ) );
                        }
                    } catch (...) { // Parsing was unsuccessful
                        LOG_WARN( "MaterialParser: Could not parse parameter '" + propName + "' as a vec4 or color. Please ensure that it is a valid vec4 or color in hex format." );
                    }
                    break;
                }
                case DataType::Texture2D:
                {
                    try
                    {
                        String path = it.value();
                        material->setTexture( propName.c_str(), ASSETS.getTexture2D( path ) );
                    } catch (...) { // Parsing was unsuccessful
                        LOG_WARN( "MaterialParser: Could not parse string for parameter '" + propName + "' (Tex2D). Please ensure that its a valid string." );
                    }
                    break;
                }
                case DataType::TextureCubemap:
                {
                    try
                    {
                        auto paths = it.value();

                        OS::Path posX = paths["posX"].get<String>();
                        OS::Path negX = paths["negX"].get<String>();
                        OS::Path posY = paths["posY"].get<String>();
                        OS::Path negY = paths["negY"].get<String>();
                        OS::Path posZ = paths["posZ"].get<String>();
                        OS::Path negZ = paths["negZ"].get<String>();
                        material->setTexture( propName.c_str(), ASSETS.getCubemap( posX, negX, posY, negY, posZ, negZ ) );
                    }
                    catch (...) { // Parsing was unsuccessful
                        LOG_WARN( "MaterialParser: Could not parse strings for parameter '" + propName + "' (Cubemap). Please ensure that they are valid strings." );
                    }
                    break;
                }
                case DataType::Matrix:
                case DataType::Vec2:
                case DataType::Vec3:
                case DataType::Char:
                case DataType::Struct:
                case DataType::String:
                case DataType::Double:
                case DataType::Texture1D:
                case DataType::Texture3D:
                    LOG_WARN( "MaterialParser: Material type of parameter '" + propName + "' is not supported" ); break;
                default:
                    LOG_WARN( "MaterialParser: Material parameter '" + propName + "' does not exist in shader '" + material->getShader()->getName() + "'" );
                }

            }
        }

    private:
        static Math::Vec4 _ParseVec4( const JSON& value )
        {
            Math::Vec4 result;
      
            if ( value.find("x") != value.end() )
                result.x = value["x"];
            else if ( value.find("r") != value.end() )
                result.x = value["r"];

            if ( value.find("y") != value.end() )
                result.y = value["y"];
            else if ( value.find("g") != value.end() )
                result.y = value["g"];

            if ( value.find("z") != value.end() )
                result.z = value["z"];
            else if ( value.find("b") != value.end() )
                result.z = value["b"];

            if ( value.find("w") != value.end() )
                result.w = value["w"];
            else if ( value.find("a") != value.end() )
                result.w = value["a"];

            return result;
        }

    };

} } // End namespaces