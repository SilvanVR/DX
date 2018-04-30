#pragma once
/**********************************************************************
    class: ShaderParser (shader_parser.hpp)

    author: S. Hau
    date: March 1, 2018
**********************************************************************/

#include "Graphics/i_shader.hpp"
#include "OS/FileSystem/file.h"
#include "Common/string_utils.h"

namespace Core { namespace Assets {

    //**********************************************************************
    class ShaderParser
    {
    public:
        static bool LoadShader( ShaderPtr shader, const OS::Path& filePath )
        {
            OS::File file( filePath, OS::EFileMode::READ );
            if ( not file.exists() )
                return false;

            shader->setName( filePath.getFileName() );

            enum class ShaderType
            {
                NONE = 0, VERTEX = 1, FRAGMENT = 2, GEOMETRY = 3, TESSELLATION = 4, NUM_SHADER_TYPES = 5
            } type = ShaderType::NONE;

            // Parse file
            std::array<String, (I32)ShaderType::NUM_SHADER_TYPES> shaderSources;
            while ( not file.eof() )
            {
                String line = file.readLine();
                if ( line.find( "#shader" ) != String::npos )
                {
                    if ( line.find( "vertex" ) != String::npos )
                        type = ShaderType::VERTEX;
                    else if ( line.find( "fragment" ) != String::npos )
                        type = ShaderType::FRAGMENT;
                    else if ( line.find( "geometry" ) != String::npos )
                        type = ShaderType::GEOMETRY;
                    else if ( line.find( "tessellation" ) != String::npos )
                        type = ShaderType::TESSELLATION;
                }
                else if ( line.find( "#include" ) != String::npos )
                {
                    auto includeFilePath = StringUtils::substringBetween( line, '\"', '\"' );
                    OS::File includeFile( filePath.getDirectoryPath() + includeFilePath );
                    if ( not includeFile.exists() )
                        LOG_WARN( "Could not include file '" + includeFilePath + "' in shader-file '" + filePath.toString() + "'." );
                    else
                        shaderSources[(I32)type].append( includeFile.readAll() );
                }
                else
                {
                    shaderSources[(I32)type].append( line + '\n' );
                }
            }

            // Compile each shader
            for (I32 i = 1; i < shaderSources.size(); i++)
            {
                if ( not shaderSources[i].empty() )
                {
                    switch ((ShaderType)i)
                    {
                    case ShaderType::VERTEX: 
                        if ( not shader->compileVertexShaderFromSource( shaderSources[i], "main" ) )
                            return false;
                        break;
                    case ShaderType::FRAGMENT: 
                        if ( not shader->compileFragmentShaderFromSource( shaderSources[i], "main" ) )
                            return false;
                        break;
                    case ShaderType::GEOMETRY:
                    case ShaderType::TESSELLATION:
                        ASSERT( "Shadertype not supported yet!" );
                    }
                }
            }

            // Parse & set pipeline states
            Graphics::RasterizationState    rzState;
            Graphics::DepthStencilState     dsState;
            Graphics::BlendState            bsState;

            StringUtils::IStringStream ss(shaderSources[0]);
            while ( not ss.eof() )
            {
                String line = StringUtils::toLower( ss.nextLine() );

                // Cull mode
                if ( line.find("cull") != String::npos )
                {
                    if (line.find("front") != String::npos)     rzState.cullMode = Graphics::CullMode::Front;
                    else if (line.find("back") != String::npos) rzState.cullMode = Graphics::CullMode::Back;
                    else if (line.find("none") != String::npos) rzState.cullMode = Graphics::CullMode::None;
                    else LOG_WARN( "Could not read cull mode in shader '" + filePath.toString() + "'." );
                }
                // Fill mode
                else if ( line.find("fill") != String::npos )
                {
                    if (line.find("solid") != String::npos)             rzState.fillMode = Graphics::FillMode::Solid;
                    else if (line.find("wireframe") != String::npos)    rzState.fillMode = Graphics::FillMode::Wireframe;
                    else LOG_WARN( "Could not read fill mode in shader '" + filePath.toString() + "'." );
                }
                // ZWrite
                else if ( line.find("zwrite") != String::npos )
                {
                    if (line.find("on") != String::npos)        dsState.depthEnable = true;
                    else if (line.find("off") != String::npos)  dsState.depthEnable = false;
                    else LOG_WARN( "Could not read zwrite in shader '" + filePath.toString() + "'." );
                }
                // ZTest
                else if ( line.find("ztest") != String::npos )
                {
                    if (line.find("never") != String::npos)             dsState.depthFunc = Graphics::ComparisonFunc::Never;
                    else if (line.find("less") != String::npos)         dsState.depthFunc = Graphics::ComparisonFunc::Less;
                    else if (line.find("equal") != String::npos)        dsState.depthFunc = Graphics::ComparisonFunc::Equal;
                    else if (line.find("lessequal") != String::npos)    dsState.depthFunc = Graphics::ComparisonFunc::LessEqual;
                    else if (line.find("greater") != String::npos)      dsState.depthFunc = Graphics::ComparisonFunc::Greater;
                    else if (line.find("notequal") != String::npos)     dsState.depthFunc = Graphics::ComparisonFunc::NotEqual;
                    else if (line.find("greaterequal") != String::npos) dsState.depthFunc = Graphics::ComparisonFunc::GreaterEqual;
                    else if (line.find("always") != String::npos)       dsState.depthFunc = Graphics::ComparisonFunc::Always;
                    else LOG_WARN( "Could not read ztest in shader '" + filePath.toString() + "'." );
                }
                // Priority
                else if ( auto pos = line.find("priority") != String::npos )
                {
                    StringUtils::IStringStream ssLine( line.substr( pos + String( "priority" ).size() ) );
                    I32 priority;
                    ssLine >> priority;

                    if ( priority == std::numeric_limits<I32>::max() )
                        LOG_WARN( "Could not read priority in shader '" + filePath.toString() + "'." );
                    else 
                        shader->setPriority( priority );
                }
                // BlendState
                else if ( auto pos = line.find("blend") != String::npos )
                {
                    StringUtils::IStringStream ssLine( line.substr( pos + String( "blend" ).size() ) );

                    String srcBlend, dstBlend;
                    ssLine >> srcBlend;
                    ssLine >> dstBlend;

                    if ( not srcBlend.empty() && not dstBlend.empty() )
                    {
                        bsState.blendStates[0].blendEnable = true;
                        bsState.blendStates[0].srcBlend = _ReadBlend(srcBlend);
                        bsState.blendStates[0].destBlend = _ReadBlend(dstBlend);
                    }
                    else
                    {
                        LOG_WARN( "Could not blend mode in shader '" + filePath.toString() + "'." );
                    }
                }
                // AlphaToMask
                else if (line.find("alphatomask") != String::npos)
                {
                    if (line.find("on") != String::npos)        bsState.alphaToCoverage = true;
                    else if (line.find("off") != String::npos)  bsState.alphaToCoverage = false;
                    else LOG_WARN( "Could not read AlphaToMask in shader '" + filePath.toString() + "'." );
                }
            }

            shader->setDepthStencilState( dsState );
            shader->setRasterizationState( rzState );
            shader->setBlendState( bsState );

            return true;
        }

    private:
        static Graphics::Blend _ReadBlend(const String& blend)
        {
            if (blend == "one")                     return Graphics::Blend::One;
            else if (blend == "zero")               return Graphics::Blend::Zero;
            else if (blend == "srccolor")           return Graphics::Blend::SrcColor;
            else if (blend == "srcalpha")           return Graphics::Blend::SrcAlpha;
            else if (blend == "dstcolor")           return Graphics::Blend::DestColor;
            else if (blend == "dstalpha")           return Graphics::Blend::DestAlpha;
            else if (blend == "oneminussrccolor")   return Graphics::Blend::InvSrcColor;
            else if (blend == "oneminussrcalpha")   return Graphics::Blend::InvSrcAlpha;
            else if (blend == "oneminusdstcolor")   return Graphics::Blend::InvDestColor;
            else if (blend == "oneminusdstalpha")   return Graphics::Blend::InvDestAlpha;
            else return Graphics::Blend::One;
        }

    };

} } // End namespaces