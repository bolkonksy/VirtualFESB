#ifndef _QUESTIONPLATFORM_H
#define _QUESTIONPLATFORM_H

#include <Engine/UserInterface.h>
#include "ISensor.h"

const Engine::Color QUESTION_COLOR = Engine::Color::Purple();

class QuestionPlatform : public ISensor {
public:
    void init(b2World* world, const glm::vec2& position, const glm::vec2& dimensions, float angle,
              const std::string& questionFile = "");
    static bool initGUI(const Engine::UserInterface& gui);

    /* Setters */
    virtual void setActive(bool value) override { _isActive = value; };
    void setSolved(bool value)                  { _isSolved = value; };

    /* Getters */
    const std::string&     questionFile() const { return _questionFile; }
    const glm::vec2        position()     const { return _hitBox.position(); }
    const glm::vec2        dimensions()   const { return _hitBox.dimensions(); }
    bool testPoint(float x, float y)      const { return _hitBox.testPoint(x, y); }
    bool isSolved()                       const { return _isSolved; }

private:
    std::string _questionFile = "";
    bool        _isSolved     = false;
};

#endif