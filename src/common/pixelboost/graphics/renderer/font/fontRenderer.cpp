#ifndef PIXELBOOST_DISABLE_GRAPHICS

#include "pixelboost/file/fileHelpers.h"

#include "pixelboost/graphics/device/device.h"
#include "pixelboost/graphics/device/indexBuffer.h"
#include "pixelboost/graphics/device/texture.h"
#include "pixelboost/graphics/device/vertexBuffer.h"
#include "pixelboost/graphics/helper/screenHelpers.h"
#include "pixelboost/graphics/renderer/font/fontRenderer.h"

using namespace pb;

FontRenderer::FontRenderer(int maxCharacters)
    : _MaxCharacters(maxCharacters)
{
    _IndexBuffer = GraphicsDevice::Instance()->CreateIndexBuffer(kBufferFormatStatic, _MaxCharacters*6);
    _VertexBuffer = GraphicsDevice::Instance()->CreateVertexBuffer(kBufferFormatDynamic, kVertexFormat_P_XYZ_UV, _MaxCharacters*4);
    
    _IndexBuffer->Lock();
    
    unsigned short* indices = _IndexBuffer->GetData();
    for (int i=0; i<_MaxCharacters; i++)
    {
        indices[0] = (i*4)+2;
        indices[1] = (i*4)+1;
        indices[2] = (i*4)+0;
        indices[3] = (i*4)+3;
        indices[4] = (i*4)+2;
        indices[5] = (i*4)+0;
        indices += 6;
    }
    
    _IndexBuffer->Unlock();
}

FontRenderer::~FontRenderer()
{
    GraphicsDevice::Instance()->DestroyIndexBuffer(_IndexBuffer);
    GraphicsDevice::Instance()->DestroyVertexBuffer(_VertexBuffer);
    
    for (FontMap::iterator it = _Fonts.begin(); it != _Fonts.end(); ++it)
    {
        delete it->second;
    }
}

void FontRenderer::AddCharacter(Vertex_PXYZ_UV* buffer, const Font::Character& character, float offset, float baseline)
{
    float xOffset = character.xOffset;
    float yOffset = -character.yOffset + baseline;
    
    buffer[0].position[0] = offset + xOffset;
    buffer[0].position[1] = yOffset - character.height;
    buffer[0].uv[0] = character.uvx;
    buffer[0].uv[1] = character.uvy + character.uvv;
    buffer[1].position[0] = offset + xOffset;
    buffer[1].position[1] = yOffset;
    buffer[1].uv[0] = character.uvx;
    buffer[1].uv[1] = character.uvy;
    buffer[2].position[0] = offset + character.width + xOffset;
    buffer[2].position[1] = yOffset;
    buffer[2].uv[0] = character.uvx + character.uvu;
    buffer[2].uv[1] = character.uvy;
    buffer[3].position[0] = offset + character.width + xOffset;
    buffer[3].position[1] = yOffset - character.height;
    buffer[3].uv[0] = character.uvx + character.uvu;
    buffer[3].uv[1] = character.uvy + character.uvv;
}

void FontRenderer::LoadFont(const std::string& name, bool createMips)
{
    if (_Fonts.find(name) != _Fonts.end())
        return;
    
    Font* font = new Font();
    
    font->texture = 0;
    
    std::string fileRoot = FileHelpers::GetRootPath();
    
    std::string fntFilename = fileRoot + "/data/fonts/" + name + (ScreenHelpers::IsHighResolution() ? "-hd" : "") + ".fnt";
    
    std::string fontContents = FileHelpers::FileToString(fntFilename);
    
    std::vector<std::string> lines;
    SplitString(fontContents, '\n', lines);
    
    for (std::vector<std::string>::iterator line = lines.begin(); line != lines.end(); ++line)
    {
        std::vector<std::string> elements;
        SplitString(*line, ' ', elements);
        
        if (elements.size() < 1 )
            continue;
        
        std::map<std::string, std::string> data;
        for (std::vector<std::string>::iterator element = elements.begin(); element != elements.end(); ++element)
        {
            data[element->substr(0, element->find('='))] = element->substr(element->find('=')+1);
        }
        
        std::string elementType = elements[0];
        
        glm::vec2 texSize;
        
        if (elementType == "info")
        {
            int size = (int)atoi(data["size"].c_str());
            
            font->size = size;
        } else if (elementType == "common")
        {
            int lineHeight = (int)atoi(data["lineHeight"].c_str());
            int base = (int)atoi(data["base"].c_str());
            int scaleW = (int)atoi(data["scaleW"].c_str());
            int scaleH = (int)atoi(data["scaleH"].c_str());
            
            font->base = (float)base/(float)font->size;
            font->lineHeight = (float)lineHeight / (float)scaleH;
            texSize = glm::vec2(scaleW, scaleH);
        } else if (elementType == "page")
        {
            std::string texFilename = fileRoot + "/data/fonts/" + data["file"].substr(1, data["file"].find('"', 1)-1);
            font->texture = GraphicsDevice::Instance()->CreateTexture();
            font->texture->LoadFromPng(texFilename, createMips);
        } else if (elementType == "char")
        {
            Font::Character character;
            
            char charCode = (char)atoi(data["id"].c_str());
            int x = (int)atoi(data["x"].c_str());
            int y = (int)atoi(data["y"].c_str());
            int width = (int)atoi(data["width"].c_str());
            int height = (int)atoi(data["height"].c_str());
            int xoffset = (int)atoi(data["xoffset"].c_str());
            int yoffset = (int)atoi(data["yoffset"].c_str());
            int xadvance = (int)atoi(data["xadvance"].c_str());
            
            character.width = width/(float)font->size;
            character.height = height/(float)font->size;
            character.uvx = x/texSize[0];
            character.uvy = y/texSize[1];
            character.uvu = width/texSize[0];
            character.uvv = height/texSize[1];
            character.xOffset = xoffset/(float)font->size;
            character.yOffset = yoffset/(float)font->size;
            character.xAdvance = xadvance/(float)font->size;
            
            font->chars[charCode] = character;
        } else if (elementType == "kerning")
        {
            char charOne = (char)atoi(data["first"].c_str());
            char charTwo = (char)atoi(data["second"].c_str());
            int amount = (int)atoi(data["amount"].c_str());
            
            font->kerning[std::pair<char, char>(charOne, charTwo)] = amount/texSize[0];
        }
    }
    
    _Fonts[name] = font;
}

void FontRenderer::Update(float time)
{
    _Instances.clear();
}

void FontRenderer::Render(RenderLayer* layer)
{
    InstanceList& instances = _Instances[layer];
    
    if (instances.size() == 0)
        return;
    
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glEnable(GL_TEXTURE_2D);
    
#ifndef PIXELBOOST_GRAPHICS_PREMULTIPLIED_ALPHA
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
#else
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
#endif
    
    glDisableClientState(GL_COLOR_ARRAY);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    
    _IndexBuffer->Bind();
    
    for (InstanceList::iterator it = instances.begin(); it != instances.end(); ++it)
    {
        Font* font;
        
        FontMap::iterator fontIt = _Fonts.find(it->_Font);
        if (fontIt == _Fonts.end() || fontIt->second->texture == 0)
        {
            continue;
        }
        
        if (it->_String.length() > _MaxCharacters)
        {
            printf("String (%s) is too long for the MaxCharacters value set\n", it->_String.c_str());
            continue;
        }
        
        font = fontIt->second;
        
        _VertexBuffer->Lock();
        
        int numCharacters = 0;
        Vertex_PXYZ_UV* vertexBuffer = static_cast<Vertex_PXYZ_UV*>(_VertexBuffer->GetData());
        
        float offset = 0.f;
        for (int i=0; i<it->_String.length(); i++)
        {
            std::map<char, Font::Character>::iterator charIt = font->chars.find(it->_String[i]);
            
            if (charIt == font->chars.end())
                continue;
            
            AddCharacter(vertexBuffer, charIt->second, offset, font->base);
            
            vertexBuffer += 4;
            numCharacters++;
            
            offset += charIt->second.xAdvance;
            
            if (i<it->_String.length()-1)
            {
                std::map<std::pair<char, char>, float>::iterator kerningIt = font->kerning.find(std::pair<char, char>(it->_String[i], it->_String[i+1]));
                
                if (kerningIt != font->kerning.end())
                    offset += kerningIt->second;
            }
        }
        
        _VertexBuffer->Unlock(numCharacters*4);
        
        _VertexBuffer->Bind();
        
        font->texture->Bind(0);
        
        glPushMatrix();
        
        glTranslatef(it->_Position[0], it->_Position[1], 0.f);

        glRotatef(it->_Rotation, 0, 0, 1);
        
        glScalef(it->_Size, it->_Size, 1.f);
        
        switch (it->_Alignment) {
            case kFontAlignLeft:
                break;
            case kFontAlignCenter:
                glTranslatef(-offset/2.f, 0, 0);
                break;
            case kFontAlignRight:
                glTranslatef(-offset, 0, 0);
                break;
        }
        
        glColor4f(it->_Color[0], it->_Color[1], it->_Color[2], it->_Color[3]);
        
        GraphicsDevice::Instance()->DrawElements(GraphicsDevice::kElementTriangles, numCharacters*6);
        
        glColor4f(1, 1, 1, 1);
        
        glPopMatrix();
    }
    
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    
    GraphicsDevice::Instance()->BindIndexBuffer(0);
    GraphicsDevice::Instance()->BindVertexBuffer(0);
    
    glDisable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);
}

bool FontRenderer::AttachToRenderer(RenderLayer* layer, const std::string& fontName, const std::string& string, glm::vec2 position, FontAlign alignment, float size, float rotation, glm::vec4 color)
{
    FontInstance instance;
    
    instance._Font = fontName;
    instance._String = string;
    instance._Position = position;
    instance._Alignment = alignment;
    instance._Rotation = rotation;
    instance._Size = size;
    instance._Color = color;
    
    _Instances[layer].push_back(instance);
    
    return true;
}

float FontRenderer::MeasureString(const std::string& fontName, const std::string& string, float size)
{
    Font* font;
    
    FontMap::iterator fontIt = _Fonts.find(fontName);
    if (fontIt == _Fonts.end() || fontIt->second->texture == 0)
        return 0.f;
    
    font = fontIt->second;
    
    float offset = 0.f;
    for (int i=0; i<string.length(); i++)
    {
        std::map<char, Font::Character>::iterator charIt = font->chars.find(string[i]);
        
        if (charIt == font->chars.end())
            continue;
        
        offset += charIt->second.xAdvance;
        
        if (i<string.length()-1)
        {
            std::map<std::pair<char, char>, float>::iterator kerningIt = font->kerning.find(std::pair<char, char>(string[i], string[i+1]));
            
            if (kerningIt != font->kerning.end())
                offset += kerningIt->second;
        }
    }
    
    return offset * size;
}

void FontRenderer::SplitString(const std::string& string, char seperator, std::vector<std::string>& output)
{
    std::string item;
    
    for (int i=0; i<string.length(); i++)
    {
        char ch=string[i];
        
        if (ch == seperator)
        {
            if (!item.empty())
            {
                output.push_back(item);
            }
            item = "";
        }
        else
        {
            item += ch;
        }
    }
    if (!item.empty())
    {
        output.push_back(item);
    }
}

#endif