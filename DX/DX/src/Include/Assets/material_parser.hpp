#pragma once
/**********************************************************************
    class: ShaderParser (shader_parser.hpp)

    author: S. Hau
    date: March 1, 2018
**********************************************************************/

#include "Graphics/i_material.h"
#include "OS/FileSystem/file.h"
#include "Ext/JSON/json.hpp"
#include "Core/locator.h"

using JSON = nlohmann::json;

namespace Assets {

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
        // @Params:
        //  "material": The material object
        //  "filePath": Path to the material file
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
                auto shader = ASSETS.getShader( shaderPath );
                material->setShader( shader );
                json.erase( "shader" );
            }
            else
            {
                LOG_WARN( "No [shader] field present in the material. The error shader will be used instead." );
                material->setShader( ASSETS.getErrorShader() );
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
                        LOG_WARN( "MaterialParser: Could not parse parameter '" + propName + "' as an int for material '" + filePath.toString() +"'. "
                                  "Please ensure that it is a valid int." );
                    }
                    break;
                }
                case DataType::Float:
                {
                    try
                    {
                        material->setFloat( propName.c_str(), it.value() );
                    } catch (...) { // Parsing was unsuccessful
                        LOG_WARN( "MaterialParser: Could not parse parameter '" + propName + "' as an float for material '" + filePath.toString() + "'. "
                                  "Please ensure that it is a valid float." );
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
                        LOG_WARN( "MaterialParser: Could not parse parameter '" + propName + "' as a vec4 or color for material '" + filePath.toString() + "'. "
                                  "Please ensure that it is a valid vec4 or color in hex format." );
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
                        LOG_WARN( "MaterialParser: Could not parse string for parameter '" + propName + "' (Tex2D) for material '" + filePath.toString() + "'. "
                                  "Please ensure that its a valid string." );
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
                        LOG_WARN( "MaterialParser: Could not parse strings for parameter '" + propName + "' (Cubemap) for material '" + filePath.toString() + "'. "
                                  "Please ensure that they are valid strings." );
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
                    LOG_WARN( "MaterialParser: Material type of parameter '" + propName + "' is not supported yet." ); break;
                default:
                    LOG_WARN( "MaterialParser: Parameter '" + propName + "' does not exist in shader '" + material->getShader()->getName() + "'"
                              " for material '" + filePath.toString() + "'");
                }
            } // End for loop

            // Assume its a PBR-material if "pbr" is found in the file-extension
            if ( filePath.getExtension().find("pbr") != String::npos )
                _SetPBRParams( material );
        }

    private:
        //----------------------------------------------------------------------
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

        //----------------------------------------------------------------------
        static void _SetPBRParams( const MaterialPtr& material )
        {
            static const StringID NAME_COLOR                = SID( "color" );
            static const StringID NAME_ROUGHNESS            = SID( "roughness" );
            static const StringID NAME_METALLIC             = SID( "metallic" );
            static const StringID NAME_ROUGHNESS_MAP        = SID( "roughnessMap" );
            static const StringID NAME_METALLIC_MAP         = SID( "metallicMap" );
            static const StringID NAME_USE_ROUGHNESS_MAP    = SID( "useRoughnessMap" );
            static const StringID NAME_USE_METALLIC_MAP     = SID( "useMetallicMap" );
            static const StringID NAME_NORMAL_MAP           = SID( "normalMap" );

            material->setFloat( NAME_USE_METALLIC_MAP, 0.0f );
            material->setFloat( NAME_USE_ROUGHNESS_MAP, 0.0f );

            // Color
            if ( not material->hasVec4( NAME_COLOR ) )
                material->setColor( NAME_COLOR, Color::WHITE );

            // Normalmap
            if ( not material->hasTexture( NAME_NORMAL_MAP ) )
                material->setTexture( NAME_NORMAL_MAP, ASSETS.getNormalTexture() );

            // Roughness
            if ( not material->hasFloat( NAME_ROUGHNESS ) )
                material->setFloat( NAME_ROUGHNESS, 0.1f );

            if ( material->hasTexture( NAME_ROUGHNESS_MAP ) )
                material->setFloat( NAME_USE_ROUGHNESS_MAP, 1.0f );
            else
                material->setTexture( NAME_ROUGHNESS_MAP, ASSETS.getBlackTexture() );

            // Metallic
            if ( not material->hasFloat( NAME_METALLIC ) )
                material->setFloat( NAME_METALLIC, 0.0f );

            if ( material->hasTexture( NAME_METALLIC_MAP ) )
                material->setFloat( NAME_USE_METALLIC_MAP, 1.0f );
            else
                material->setTexture( NAME_METALLIC_MAP, ASSETS.getBlackTexture() );
        }

        MaterialParser() = delete;
    };

} // End namespaces