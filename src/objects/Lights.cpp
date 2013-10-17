#include "Lights.hpp"

Lights::Lights(int maxLights) :
    m_lightCount(0),
    m_lights(maxLights),
    m_idxMap(maxLights, INVALID_INDEX)
{
    for ( int i = 0; i < maxLights; ++i )
        m_availableIdx.push(i);
}

int Lights::addLight(const LightInfo& light)
{
    int idx = INVALID_INDEX;

    if ( !m_availableIdx.empty() )
    {
        idx = m_availableIdx.front();
        m_availableIdx.pop();

        m_lights[m_lightCount] = light;
        m_idxMap[idx] = m_lightCount;
        m_lightCount++;
        return idx;
    }

    return INVALID_INDEX;
}

bool Lights::setLightInfo(const LightInfo& light, size_t idx)
{
    if ( idx < m_idxMap.size() && m_idxMap[idx] != INVALID_INDEX )
    {
        m_lights[m_idxMap[idx]] = light;
        return true;
    }
    return false;
}

bool Lights::setPosition(const glm::vec3 &position, size_t idx)
{
    if ( idx < m_idxMap.size() && m_idxMap[idx] != INVALID_INDEX )
    {
        m_lights[m_idxMap[idx]].position = position;
        return true;
    }
    return false; 
}

bool Lights::setDiffuse(const glm::vec3 &diffuse, size_t idx)
{
    if ( idx < m_idxMap.size() && m_idxMap[idx] != INVALID_INDEX )
    {
        m_lights[m_idxMap[idx]].diffuse = diffuse;
        return true;
    }
    return false; 
}

bool Lights::setSpecular(const glm::vec3 &specular, size_t idx)
{
    if ( idx < m_idxMap.size() && m_idxMap[idx] != INVALID_INDEX )
    {
        m_lights[m_idxMap[idx]].specular = specular;
        return true;
    }
    return false; 
}

bool Lights::setAmbient(const glm::vec3 &ambient, size_t idx)
{
    if ( idx < m_idxMap.size() && m_idxMap[idx] != INVALID_INDEX )
    {
        m_lights[m_idxMap[idx]].ambient = ambient;
        return true;
    }
    return false; 
}

int Lights::getLightCount() const
{
    return m_lightCount;
}

bool Lights::getLightInfo(LightInfo& info, size_t idx) const
{
    if ( idx < m_idxMap.size() && m_idxMap[idx] != INVALID_INDEX )
    {
        info = m_lights[m_idxMap[idx]];
        return true;
    }
    return false;
}

bool Lights::lightExists(size_t idx) const
{
    if ( idx < m_idxMap.size() )
        return m_idxMap[idx] != INVALID_INDEX;
    return false;
}

bool Lights::removeLight(size_t idx)
{
    if ( idx < m_idxMap.size() && m_idxMap[idx] != INVALID_INDEX )
    {
        m_lights.erase(m_lights.begin() + m_idxMap[idx]);
        m_lights.resize(m_lights.size()+1);
        m_idxMap[idx] = INVALID_INDEX;
        m_availableIdx.push(idx);
        m_lightCount--;
        return true;
    }
    return false;
}

void Lights::loadCount(GLBuffer& buffer, int bufferIdx)
{
    buffer.bind(GL_UNIFORM_BUFFER, bufferIdx);
    buffer.setSubData(&m_lightCount, LIGHT_COUNT_OFFSET, 1, bufferIdx);
    GLBuffer::unbindBuffers(GL_UNIFORM_BUFFER);
}

bool Lights::loadLight(GLBuffer& buffer, int bufferIdx, const glm::mat4& viewMatrix, size_t idx)
{
    if ( idx < m_idxMap.size() && m_idxMap[idx] != INVALID_INDEX )
    {
        LightInfo& info = m_lights[m_idxMap[idx]];

        // compute light position in view coordinates
        glm::vec3 position = (viewMatrix * glm::vec4(info.position, 1.0)).xyz();

        // set buffer data
        buffer.bind(GL_UNIFORM_BUFFER, bufferIdx);
        buffer.setSubData(&position, LIGHT_ARRAY_OFFSET + m_idxMap[idx] * LIGHT_ARRAY_STEP + LIGHT_ARRAY_POSITION_OFFSET, 1, bufferIdx);
        buffer.setSubData(&info.diffuse, LIGHT_ARRAY_OFFSET + m_idxMap[idx] * LIGHT_ARRAY_STEP + LIGHT_ARRAY_DIFFUSE_OFFSET, 1, bufferIdx);
        buffer.setSubData(&info.specular, LIGHT_ARRAY_OFFSET + m_idxMap[idx] * LIGHT_ARRAY_STEP + LIGHT_ARRAY_SPECULAR_OFFSET, 1, bufferIdx);
        buffer.setSubData(&info.ambient, LIGHT_ARRAY_OFFSET + m_idxMap[idx] * LIGHT_ARRAY_STEP + LIGHT_ARRAY_AMBIENT_OFFSET, 1, bufferIdx);
        GLBuffer::unbindBuffers(GL_UNIFORM_BUFFER);
        
        return true;
    }
    return false;
}

void Lights::load(GLBuffer& buffer, int bufferIdx, const glm::mat4& viewMatrix)
{
    buffer.bind(GL_UNIFORM_BUFFER, bufferIdx);
    buffer.setSubData(&m_lightCount, LIGHT_COUNT_OFFSET, 1, bufferIdx);
    for ( size_t i = 0; i < m_lights.size(); ++i )
    {
        LightInfo& info = m_lights[i];

        // compute light position in view coordinates
        glm::vec3 position = (viewMatrix * glm::vec4(info.position, 1.0)).xyz();

        // set buffer data
        buffer.setSubData(&position, LIGHT_ARRAY_OFFSET + i * LIGHT_ARRAY_STEP + LIGHT_ARRAY_POSITION_OFFSET, 1, bufferIdx);
        buffer.setSubData(&info.diffuse, LIGHT_ARRAY_OFFSET + i * LIGHT_ARRAY_STEP + LIGHT_ARRAY_DIFFUSE_OFFSET, 1, bufferIdx);
        buffer.setSubData(&info.specular, LIGHT_ARRAY_OFFSET + i * LIGHT_ARRAY_STEP + LIGHT_ARRAY_SPECULAR_OFFSET, 1, bufferIdx);
        buffer.setSubData(&info.ambient, LIGHT_ARRAY_OFFSET + i * LIGHT_ARRAY_STEP + LIGHT_ARRAY_AMBIENT_OFFSET, 1, bufferIdx);
    }
    GLBuffer::unbindBuffers(GL_UNIFORM_BUFFER);
}

