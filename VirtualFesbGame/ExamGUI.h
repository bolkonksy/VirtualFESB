#ifndef _EXAMGUI_H
#define _EXAMGUI_H

#define _GAME_MACROS_ENABLED  /* Defined in Game.h */

#include <fstream>
#include <Engine/UserInterface.h>
#include "Objects/QuestionPlatform.h"
#include "Language.h"

class Game;

class ExamGUI {
private:

    struct Answer {
        bool Correct = false;
        std::string Text = "";
    };

    class Question {
    public:
        Question() : Answers(4) {}
        std::string Text  = "";
        std::vector<Answer> Answers;
    };

    class Exam {
    public:
        std::string QuestionFile = "";
        Lang Language = Lang::ENG;

        /* Header */
        std::string ExamName   = "";
        const size_t QHL       = 5; /* Question Header Length */
        size_t TotalQuestions  = 0; /* Total questions in question file, not in exam */
        size_t CurrentQuestion = 0; /* Current question from 0 to 6 (exam contains 7 questions)*/
        size_t CorrectAnswers  = 0;

        /* Loaded Questions */
        std::vector<Question> Questions;

        void clear();  /* Reset members to default value */

        bool loadExam(const std::string& filePath, Lang lang = Lang::ENG);
    private:
        bool loadHeaderInfo(std::ifstream& file);
        bool loadQuestions(std::ifstream& file);
        bool loadQuestion(std::ifstream& file, size_t questionIndex);
    };

public:
    bool init(Engine::UserInterface& gui, size_t questionPlatformCount, Game* game);
    void update();

    /* Setters */
    void cancelExam();
    void activate(QuestionPlatform& QP, const Lang& lang);

    /* Getters */
    bool isActive() const { return _active; }

private:
    bool initGUI(Engine::UserInterface& gui);
    void setAnswerText(size_t questionIndex);
    void clearSelectedAnswer();
    void updateTotalResult();
    void showResults();

    /* CEGUI Events */
    bool cancelButtonClicked       (const CEGUI::EventArgs& e);
    bool nextButtonClicked         (const CEGUI::EventArgs& e);

    /* Members */
    Game* _game = nullptr;
    Exam _exam;
    QuestionPlatform* _currentQuestion = nullptr;

    int  _examsCount  = 0; /* Number of total exams in the level */
    int  _solvedExams = 0; /* Number of solved exams */
    bool _active      = false;

    /* * * CEGUI * * */
    CEGUI::FrameWindow* _examWindow   = nullptr;
    CEGUI::Window*      _questionText = nullptr;

    /* RadioButtons for quiz */
    CEGUI::GroupBox*    _quizGroup   = nullptr;
    CEGUI::RadioButton* _answerRB[4] = { nullptr };

    /* Other UI */
    CEGUI::PushButton*  _cancelButton       = nullptr;
    CEGUI::PushButton*  _nextQuestionButton = nullptr;
    CEGUI::ProgressBar* _examProgress       = nullptr;
};

#endif