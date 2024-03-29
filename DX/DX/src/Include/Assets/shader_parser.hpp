#pragma once
/**********************************************************************
    class: ShaderParser (shader_parser.hpp)

    author: S. Hau
    date: March 1, 2018
**********************************************************************/

#include "Graphics/i_shader.h"
#include "OS/FileSystem/file.h"
#include "Common/string_utils.h"
#include <sstream>

#define SHADER_NAME                     "#shader"
#define VERTEX_SHADER                   "vertex"
#define FRAGMENT_SHADER                 "fragment"
#define GEOMETRY_SHADER                 "geometry"

#define RASTERIZATION_CULL                      "#cull"
#define RASTERIZATION_CULL_FRONT                "front"
#define RASTERIZATION_CULL_BACK                 "back"
#define RASTERIZATION_CULL_NONE                 "none"
#define RASTERIZATION_FILL                      "#fill"
#define RASTERIZATION_FILL_SOLID                "solid"
#define RASTERIZATION_FILL_WIREFRAME            "wireframe"
#define RASTERIZATION_SCISSOR                   "#scissor"
#define RASTERIZATION_DEPTH_BIAS                "#depthbias"
#define RASTERIZATION_SLOPE_SCALED_DEPTH_BIAS   "#dbslopescaled"
#define RASTERIZATION_DEPTH_BIAS_CLAMP          "#dbclamp"
#define RASTERIZATION_DEPTH_CLIP                "#depthclip"

#define DEPTH_STENCIL_ZWRITE            "#zwrite"
#define DEPTH_STENCIL_ZTEST             "#ztest"

#define BLEND                           "#blend"
#define BLEND_ALPHA_TO_MASK             "#alphatomask"

#define SHADER_QUEUE                    "#queue"

#define INCLUDE_NAME                    "#include"

#define D3D11_NAME                       "#d3d11"
#define VULKAN_NAME                      "#vulkan"

namespace Assets {

    //**********************************************************************
    class ShaderParser
    {
        enum ShaderMapping
        {
            None     = 0,
            Vertex   = 1,
            Fragment = 2,
            Geometry = 3,
            NUM_SHADER_TYPES
        };

    public:
        //----------------------------------------------------------------------
        // Tries to load a custom shader file format from the given file.
        // @Return:
        //  A new shader object if everything was successful. 
        // @Throws:
        //  std::runtime_error if something went wrong.
        //----------------------------------------------------------------------
        static ShaderPtr LoadShader( const OS::Path& filePath )
        {
            auto shader = RESOURCES.createShader();
            shader->setName( filePath.getFileName() );
            UpdateShader( shader, filePath );
            return shader;
        }

        //----------------------------------------------------------------------
        // Updates the given shader from the given file.
        // @Throws:
        //  std::runtime_error if something went wrong.
        //----------------------------------------------------------------------
        static void UpdateShader( const ShaderPtr& shader, const OS::Path& filePath )
        {
            if ( filePath.getExtension() != "shader" )
                throw std::runtime_error( "File has wrong extension. Must be '.shader'." );

            // Parse shader file
            std::array<String, NUM_SHADER_TYPES> shaderSources = _SplitShaderFile( filePath );
            if (shaderSources[ShaderMapping::Vertex].empty())
                throw std::runtime_error( "Vertex shader source is empty. Forgot to add #d3d11 or #vulkan?" );

            // Compile each shader
            for (I32 i = 1; i < shaderSources.size(); i++)
            {
                if ( not shaderSources[i].empty() )
                {
                    switch ((ShaderMapping)(i))
                    {
                    case ShaderMapping::Vertex:   shader->compileVertexShaderFromSource( shaderSources[i], "main" ); break;
                    case ShaderMapping::Fragment: shader->compileFragmentShaderFromSource( shaderSources[i], "main" ); break;
                    case ShaderMapping::Geometry: shader->compileGeometryShaderFromSource( shaderSources[i], "main" ); break;
                    ASSERT( "Shadertype not supported yet!" );
                    }
                }
            }

            // Parse & set pipeline states
            _SetRasterizationState( shader, shaderSources[0], filePath );
            _SetDepthStencilState( shader, shaderSources[0], filePath );
            _SetBlendState( shader, shaderSources[0], filePath );
            _SetShaderPriority( shader, shaderSources[0], filePath );

            // Create pipeline & reflect resources
            shader->createPipeline();
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

        //----------------------------------------------------------------------
        static std::array<String, NUM_SHADER_TYPES> _SplitShaderFile( const OS::Path& filePath )
        {
            OS::BinaryFile file( filePath, OS::EFileMode::READ );

            auto api = Graphics::API::Unknown;
            auto type = ShaderMapping::None;
            std::array<String, NUM_SHADER_TYPES> shaderSources;
            while ( not file.eof() )
            {
                String line = file.readLine();

                if (line.size() >= 2 && line.substr(0, 2) == "//") // Skip any comments
                    continue;

                if ( line.find( D3D11_NAME ) != String::npos )
                {
                    api = Graphics::API::D3D11;
                }
                else if ( line.find( VULKAN_NAME ) != String::npos )
                {
                    api = Graphics::API::Vulkan;
                }
                else if ( line.find( SHADER_NAME ) != String::npos )
                {
                    if (api == Graphics::API::Unknown)
                        throw std::runtime_error( "ShaderParser(): No #API specification before a #shader. Please add '#vulkan' or '#d3d11' before." );

                    if (line.find( VERTEX_SHADER ) != String::npos)
                        type = Vertex;
                    else if (line.find( FRAGMENT_SHADER ) != String::npos)
                        type = Fragment;
                    else if (line.find( GEOMETRY_SHADER ) != String::npos)
                        type = Geometry;
                }
                else if ( line.find( INCLUDE_NAME ) != String::npos && api == RENDERER.getAPI() )
                {
                    auto includeFilePath = StringUtils::substringBetween( line, '\"', '\"' );
                    try
                    {
                        bool isVirtualPath = includeFilePath[0] == '/';
                        OS::Path fullPath = isVirtualPath ? includeFilePath : filePath.getDirectoryPath() + includeFilePath;

                        OS::BinaryFile includeFile( fullPath, OS::EFileMode::READ );
                        shaderSources[type].append( includeFile.readAll() );
                    }
                    catch (const std::runtime_error& e) {
                        LOG_WARN( "Could not include file '" + includeFilePath + "' in shader-file '" + filePath.toString() + "'. Reason: " + e.what() );
                    }
                }
                else
                {
                    if (api == RENDERER.getAPI() || api == Graphics::API::Unknown)
                        shaderSources[type].append( line + '\n' );
                }
            }

            return shaderSources;
        }

        //----------------------------------------------------------------------
        static inline void _SetRasterizationState( ShaderPtr shader, const String& src, const OS::Path& filePath )
        {
            Graphics::RasterizationState rzState;

            StringUtils::IStringStream ss( src );
            while ( not ss.eof() )
            {
                String line = StringUtils::toLower( ss.nextLine() );

                // Cull mode
                if ( line.find( RASTERIZATION_CULL ) != String::npos )
                {
                    if (line.find( RASTERIZATION_CULL_FRONT ) != String::npos)     rzState.cullMode = Graphics::CullMode::Front;
                    else if (line.find( RASTERIZATION_CULL_BACK ) != String::npos) rzState.cullMode = Graphics::CullMode::Back;
                    else if (line.find( RASTERIZATION_CULL_NONE ) != String::npos) rzState.cullMode = Graphics::CullMode::None;
                    else LOG_WARN( "Could not read cull mode in shader '" + filePath.toString() + "'." );
                }
                // Fill mode
                else if ( line.find( RASTERIZATION_FILL ) != String::npos )
                {
                    if (line.find( RASTERIZATION_FILL_SOLID ) != String::npos)          rzState.fillMode = Graphics::FillMode::Solid;
                    else if (line.find( RASTERIZATION_FILL_WIREFRAME ) != String::npos) rzState.fillMode = Graphics::FillMode::Wireframe;
                    else LOG_WARN( "Could not read fill mode in shader '" + filePath.toString() + "'." );
                }
                // Scissor state
                else if ( line.find( RASTERIZATION_SCISSOR ) != String::npos )
                {
                    if (line.find( "on" ) != String::npos)        rzState.scissorEnable = true;
                    else if (line.find( "off" ) != String::npos)  rzState.scissorEnable = false;
                    else LOG_WARN( "Could not read scissor state in shader '" + filePath.toString() + "'." );
                }
                // Depth bias
                else if ( line.find( RASTERIZATION_DEPTH_BIAS ) != String::npos )
                {
                    auto pos = line.find( RASTERIZATION_DEPTH_BIAS );
                    StringUtils::IStringStream ssLine( line.substr( pos + String( RASTERIZATION_DEPTH_BIAS ).size() ) );
                    ssLine >> rzState.depthBias;
                    if ( ssLine.failed() )
                    {
                        LOG_WARN( "Could not read depth bias in shader '" + filePath.toString() + "'." );
                        rzState.depthBias = 0;
                    }
                }
                // Depth bias slope scale
                else if ( line.find( RASTERIZATION_SLOPE_SCALED_DEPTH_BIAS ) != String::npos )
                {
                    auto pos = line.find( RASTERIZATION_SLOPE_SCALED_DEPTH_BIAS );
                    StringUtils::IStringStream ssLine( line.substr( pos + String( RASTERIZATION_SLOPE_SCALED_DEPTH_BIAS ).size() ) );
                    ssLine >> rzState.slopeScaledDepthBias;
                    if ( ssLine.failed() )
                    {
                        LOG_WARN( "Could not read depth bias slope scale in shader '" + filePath.toString() + "'." );
                        rzState.slopeScaledDepthBias = 0.0f;
                    }
                }
                // Depth bias clamp
                else if ( line.find( RASTERIZATION_DEPTH_BIAS_CLAMP ) != String::npos )
                {
                    auto pos = line.find( RASTERIZATION_DEPTH_BIAS_CLAMP );
                    StringUtils::IStringStream ssLine( line.substr( pos + String( RASTERIZATION_DEPTH_BIAS_CLAMP ).size() ) );
                    ssLine >> rzState.depthBiasClamp;
                    if ( ssLine.failed() )
                    {
                        LOG_WARN( "Could not read depth bias clamp in shader '" + filePath.toString() + "'." );
                        rzState.depthBiasClamp = 0.0f;
                    }
                }
                // Depth clip
                else if ( line.find( RASTERIZATION_DEPTH_CLIP ) != String::npos )
                {
                    if (line.find("on") != String::npos)        rzState.depthClipEnable = true;
                    else if (line.find("off") != String::npos)  rzState.depthClipEnable = false;
                    else LOG_WARN( "Could not read depth clip in shader '" + filePath.toString() + "'." );
                }
            }

            shader->setRasterizationState( rzState );
        }

        //----------------------------------------------------------------------
        static inline void _SetDepthStencilState( ShaderPtr shader, const String& src, const OS::Path& filePath )
        {
            Graphics::DepthStencilState dsState;

            StringUtils::IStringStream ss( src );
            while ( not ss.eof() )
            {
                String line = StringUtils::toLower( ss.nextLine() );

                // ZWrite
                if ( line.find( DEPTH_STENCIL_ZWRITE ) != String::npos )
                {
                    if (line.find("on") != String::npos)        dsState.depthWrite = true;
                    else if (line.find("off") != String::npos)  dsState.depthWrite = false;
                    else LOG_WARN( "Could not read zwrite in shader '" + filePath.toString() + "'." );
                }
                // ZTest
                else if ( line.find( DEPTH_STENCIL_ZTEST ) != String::npos )
                {
                    if (line.find("off") != String::npos)               dsState.depthEnable = false;
                    else if (line.find("never") != String::npos)        dsState.depthFunc = Graphics::ComparisonFunc::Never;
                    else if (line.find("less") != String::npos)         dsState.depthFunc = Graphics::ComparisonFunc::Less;
                    else if (line.find("equal") != String::npos)        dsState.depthFunc = Graphics::ComparisonFunc::Equal;
                    else if (line.find("lessequal") != String::npos)    dsState.depthFunc = Graphics::ComparisonFunc::LessEqual;
                    else if (line.find("greater") != String::npos)      dsState.depthFunc = Graphics::ComparisonFunc::Greater;
                    else if (line.find("notequal") != String::npos)     dsState.depthFunc = Graphics::ComparisonFunc::NotEqual;
                    else if (line.find("greaterequal") != String::npos) dsState.depthFunc = Graphics::ComparisonFunc::GreaterEqual;
                    else if (line.find("always") != String::npos)       dsState.depthFunc = Graphics::ComparisonFunc::Always;
                    else LOG_WARN( "Could not read ztest in shader '" + filePath.toString() + "'." );
                }
            }

            shader->setDepthStencilState( dsState );
        }

        //----------------------------------------------------------------------
        static inline void _SetBlendState( ShaderPtr shader, const String& src, const OS::Path& filePath )
        {
            bool found = false;
            Graphics::BlendState bsState;

            StringUtils::IStringStream ss( src );
            while ( not ss.eof() )
            {
                String line = StringUtils::toLower( ss.nextLine() );

                // BlendState
                auto pos = line.find( BLEND );
                if ( pos != String::npos )
                {
                    StringUtils::IStringStream ssLine( line.substr( pos + String( BLEND ).size() ) );

                    String srcBlend, dstBlend;
                    ssLine >> srcBlend;
                    ssLine >> dstBlend;

                    if ( not srcBlend.empty() && not dstBlend.empty() )
                    {
                        found = true;
                        bsState.blendStates[0].blendEnable = true;
                        bsState.blendStates[0].srcBlend = _ReadBlend( srcBlend );
                        bsState.blendStates[0].destBlend = _ReadBlend( dstBlend );
                    }
                    else
                    {
                        LOG_WARN( "Could not read blend mode in shader '" + filePath.toString() + "'." );
                    }
                }
                // AlphaToMask
                else if (line.find( BLEND_ALPHA_TO_MASK ) != String::npos)
                {
                    found = true;
                    if (line.find("on") != String::npos)        bsState.alphaToCoverage = true;
                    else if (line.find("off") != String::npos)  bsState.alphaToCoverage = false;
                    else LOG_WARN( "Could not read AlphaToMask in shader '" + filePath.toString() + "'." );
                }
            }

            shader->setBlendState( bsState );
        }

        //----------------------------------------------------------------------
        static inline void _SetShaderPriority( ShaderPtr shader, const String& src, const OS::Path& filePath )
        {
            StringUtils::IStringStream ss( src );
            while ( not ss.eof() )
            {
                String line = StringUtils::toLower( ss.nextLine() );

                // Priority
                auto pos = line.find( SHADER_QUEUE );
                if ( pos != String::npos )
                {
                    StringUtils::IStringStream ssLine( line.substr( pos + String( SHADER_QUEUE ).size() ) );

                    I32 queue;
                    if ( ssLine >> queue )
                        shader->setRenderQueue( queue );
                    else
                    {
                        StringUtils::IStringStream ssLine2( line.substr( pos + String( SHADER_QUEUE ).size() ) );
                        String queueAsString;
                        ssLine2 >> queueAsString;

                        queue = Graphics::StringToRenderQueue( StringUtils::toLower( queueAsString ) );
                        if ( queue != -1 && not ssLine2.failed() )
                            shader->setRenderQueue( queue );
                        else
                            LOG_WARN( "Could not read queue in shader '" + filePath.toString() + "'." );
                    }
                }
            }
        }

        ShaderParser() = delete;
    };

} // End namespaces