#pragma once

#include <map>
#include <string>
#include <vector>

#include "glm/glm.hpp"

#include "pixelboost/graphics/renderer/common/irenderer.h"
#include "pixelboost/misc/pointer.h"

namespace pb
{
    
class IndexBuffer;
class ParticleModifier;
class RenderLayer;
class SpriteSheet;
class VertexBuffer;
    
class ParticleEmitter
{
protected:
    ParticleEmitter(int maxParticles=50);
    ~ParticleEmitter();
    
    void Update(float time);
    void Render();
    
public:
    struct Config
    {
        Config();
        ~Config();
        
        int refCount;
        
        float particlesPerUpdate;
        
        float initialScale;
        float life;
        
        glm::vec2 startScale;
        glm::vec2 endScale;
        glm::vec4 startColor;
        glm::vec4 endColor;
        glm::vec2 minPosOffset;
        glm::vec2 maxPosOffset;
        glm::vec3 minRotOffset;
        glm::vec3 maxRotOffset;
        glm::vec3 minRotVelocity;
        glm::vec3 maxRotVelocity;
        glm::vec2 minPosVelocity;
        glm::vec2 maxPosVelocity;
        glm::vec2 gravity;
        
        typedef std::vector<std::string> SpriteList;
        SpriteList sprites;
        
    private:
        std::shared_ptr<SpriteSheet> spriteSheet;
        
        friend class ParticleEmitter;
        friend class ParticleRenderer;
    };
    
private:
    struct Particle
    {
        Particle(ParticleEmitter::Config* config);
        Particle(const Particle& particle);
        ~Particle();
        
        Particle& operator=(const Particle& rhs);
        
        std::string sprite;
        
        ParticleEmitter::Config* emitterConfig;
        
        float life;
        float totalLife;
        
        glm::vec3 rotation;
        glm::vec2 position;
        
        glm::vec3 rotationVelocity;
        glm::vec2 positionVelocity;
        
    private:
        void Assign(const Particle& rhs);
    };

public:
    typedef std::vector<ParticleModifier*> ModifierList;
    typedef std::vector<Particle> ParticleList;
    
    void AddModifier(ParticleModifier* modifier);
    
    bool Load(const std::string& file);
    
    void LoadSpriteSheet(const std::string& file, bool createMips);
    void SetSpriteSheet(std::shared_ptr<SpriteSheet> spriteSheet);
    
    glm::vec2 GetPosition();
    void SetPosition(const glm::vec2& position);
    
    Config& GetConfig();
    ParticleList& GetParticles();
    
private:
    static bool ParticleSortPredicate(const Particle& a, const Particle& b);
    ParticleList _Particles;
    
    ModifierList _Modifiers;
    
    Config* _Config;
    glm::vec2 _Position;
    float _EmitCount;
    
    IndexBuffer* _IndexBuffer;
    VertexBuffer* _VertexBuffer;
    
    bool _BufferDirty;
    int _MaxParticles;
    
    friend class ParticleModifier;
    friend class ParticleRenderer;
};
    
class ParticleModifier
{
public:
    ParticleModifier(ParticleEmitter* emitter);
    virtual ~ParticleModifier();
                     
    void Update(float time);
                     
protected:
    virtual void UpdateParticles(float time, ParticleEmitter::ParticleList& particles) = 0;
    
private:
    ParticleEmitter* _Emitter;
};
                                          
class ParticleAttractor : public ParticleModifier
{
public:
    ParticleAttractor(ParticleEmitter* emitter, const glm::vec2& position, float strength);
    virtual ~ParticleAttractor();
    
    virtual void UpdateParticles(float time, ParticleEmitter::ParticleList& particles);
    
private:
    glm::vec2 _Position;
    float _Strength;
};
    
class ParticleRenderer : public IRenderer
{
public:
    ParticleRenderer();
    ~ParticleRenderer();
    
    ParticleEmitter* CreateEmitter(RenderLayer* layer, int maxParticles=50);
    void DestroyEmitter(ParticleEmitter* emitter);
    
    virtual void Update(float time);
    virtual void Render(RenderLayer* layer);
    
private:
    typedef std::vector<ParticleEmitter*> EmitterList;
    typedef std::map<RenderLayer*, EmitterList> EmitterListMap;
    
    EmitterListMap _Emitters;
};

}