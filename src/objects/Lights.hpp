#ifndef LIGHTS_HPP
#define LIGHTS_HPP

#define GLM_SWIZZLE

#include "../interfaces/iGLRenderable.hpp"
#include "../glwrappers/GLBuffer.hpp"
#include <glm/glm.hpp>
#include <vector>
#include <queue>

const int INVALID_INDEX = -1;

struct LightInfo
{
    glm::vec3 position;
    glm::vec3 diffuse;
    glm::vec3 specular;
    glm::vec3 ambient;
};

class Lights
{
public:
    Lights(int maxLights = LIGHT_ARRAY_SIZE);

    // returns -1 if maxLights already exist
    int addLight(const LightInfo& light);
    bool setLightInfo(const LightInfo& light, size_t idx);

    bool setPosition(const glm::vec3 &position, size_t idx);
    bool setDiffuse(const glm::vec3 &diffuse, size_t idx);
    bool setSpecular(const glm::vec3 &specular, size_t idx);
    bool setAmbient(const glm::vec3 &ambient, size_t idx);

    int getLightCount() const;
    bool getLightInfo(LightInfo& info, size_t idx) const;
    bool lightExists(size_t idx) const;

    bool removeLight(size_t idx);

    void loadCount(GLBuffer& ubo, int uboIdx);
    bool loadLight(GLBuffer& ubo, int uboIdx, const glm::mat4& viewMatrix, size_t idx);
    void load(GLBuffer& buffer, int uboIdx, const glm::mat4& viewMatrix);
protected:
    GLint m_lightCount;
    std::vector<LightInfo> m_lights;
    std::queue<size_t>     m_availableIdx;
    std::vector<int>       m_idxMap;
};

#endif // LIGHTS_HPP

