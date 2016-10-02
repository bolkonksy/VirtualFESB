#include "ExamGUI.h"

#include <algorithm>
#include <random>
#include <time.h>
#include <limits>
#include "Game.h"

#define English     (Language == Lang::ENG)
#define Croatian    (Language == Lang::CRO)

std::mt19937 randEngine = std::mt19937{};

/* Jumping to a specific line */
std::ifstream& GoToLine(std::ifstream& file, size_t num)
{
    file.seekg(std::ios::beg);
    for (size_t i = 0; i < num - 1; i++) {
        #undef max
        file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
    return file;
}

/* Default parameters: lang = Lang::ENG */
bool ExamGUI::init(Engine::UserInterface& gui, size_t questionPlatformCount, Game* game)
{
    _examsCount = questionPlatformCount;
    _game = game;

    randEngine.seed((unsigned int)time(0));

    return initGUI(gui);
}

void ExamGUI::update()
{
    if (_answerRB[0]->isSelected() == true || _answerRB[1]->isSelected() == true ||
        _answerRB[2]->isSelected() == true || _answerRB[3]->isSelected() == true)
        _nextQuestionButton->enable();
    else 
        _nextQuestionButton->disable();
}

void ExamGUI::cancelExam()
{
    _active = false;
    _exam.clear();
    _examProgress->setProgress(0.0f);
    _examWindow->hide();
    _currentQuestion = nullptr;
    clearSelectedAnswer();
}

void ExamGUI::activate(QuestionPlatform& QP, const Lang& lang)
{
    _active = true;
    _currentQuestion = &QP;
    _exam.loadExam(QP.questionFile(), lang);
    _examWindow->show();

    _quizGroup->setText(language_("CHOOSE_ANSWER"));
    _cancelButton->setText(language_("CANCEL"));
    _nextQuestionButton->setText(language_("NEXT_QUESTION"));

    /* Show First Question */
    _examWindow->setText(_exam.ExamName);
    setAnswerText(_exam.CurrentQuestion);
    _questionText->setText(_exam.Questions[_exam.CurrentQuestion].Text);
}

void ExamGUI::Exam::clear()
{
    QuestionFile    = "";
    ExamName        = "";
    TotalQuestions  = 0;
    CurrentQuestion = 0;
    CorrectAnswers  = 0;
    Questions.clear();
}

bool ExamGUI::Exam::loadExam(const std::string& filePath, Lang lang)
{
    Language = lang;
    QuestionFile = filePath;

    std::ifstream file(filePath);
    if (file.fail())
        return false;

    /* Load header */
    if (!loadHeaderInfo(file)) {
        file.close();
        return false;
    }

    /* Load seven random questions and randomize answers */
    if (!loadQuestions(file)) {
        file.close();
        return false;
    }

    if (file.is_open())
        file.close();
    return true;
}

bool ExamGUI::Exam::loadHeaderInfo(std::ifstream& file)
{
    std::string tmp;

    if (English) {
        std::getline(file, ExamName);
        std::getline(file, tmp);
    }
    else if (Croatian) {
        std::getline(file, tmp);
        std::getline(file, ExamName);
    }
    file >> TotalQuestions;

    /* Limit number of questions between 7 and 21 */
    if (TotalQuestions < 7)
        return false;
    else if (TotalQuestions > 21)
        TotalQuestions = 21;

    return true;
}

bool ExamGUI::Exam::loadQuestions(std::ifstream& file)
{
    /* Seven questions per exam */
    Questions.reserve(7);

    /* VectorIndex is used for indexing and randomizing loaded questions */
    std::vector<size_t> questionIndex;

    /* VectorIndex conatins: 1, 2, 3, ...TotalQuestions */
    questionIndex.reserve((size_t)TotalQuestions);
    for (size_t i = 1; i <= TotalQuestions; i++) {
        questionIndex.push_back(i);
    }

    /* Shuffle vector elements */
    std::shuffle(std::begin(questionIndex), std::end(questionIndex), randEngine);
    
    while (questionIndex.size() != TotalQuestions - 7) {
        size_t loadQuestionIndex = questionIndex.back();
        questionIndex.pop_back();
        if (!loadQuestion(file, loadQuestionIndex))
            return false;
    }

    return true;
}

bool ExamGUI::Exam::loadQuestion(std::ifstream& file, size_t questionIndex)
{
    Question newQuestion;

    /* Skip question header */
    const size_t offset = QHL + 1;
    /* Calculate line position */
    const size_t line = (questionIndex - 1) * 11 + offset;

    GoToLine(file, line);
    std::string tmp;

    if (English) {
        /* Get Question text */
        std::getline(file, newQuestion.Text);
        std::getline(file, tmp);

        /* Get Correct Answer */
        std::getline(file, newQuestion.Answers[0].Text);
        newQuestion.Answers[0].Correct = true;
        std::getline(file, tmp);

        /* Get 3 Wrong Answers */
        for (size_t i = 1; i < newQuestion.Answers.size(); i++) {
            std::getline(file, newQuestion.Answers[i].Text);
            newQuestion.Answers[i].Correct = false;
            std::getline(file, tmp);
        }
    }
    else if (Croatian) {
        /* Get Question text */
        std::getline(file, tmp);
        std::getline(file, newQuestion.Text);

        /* Get Correct Answer */
        std::getline(file, tmp);
        std::getline(file, newQuestion.Answers[0].Text);
        newQuestion.Answers[0].Correct = true;

        /* Get 3 Wrong Answers */
        for (size_t i = 1; i < newQuestion.Answers.size(); i++) {
            std::getline(file, tmp);
            std::getline(file, newQuestion.Answers[i].Text);
            newQuestion.Answers[i].Correct = false;
        }
    }

    /* Randomize answers */
    std::shuffle(std::begin(newQuestion.Answers), std::end(newQuestion.Answers), randEngine);

    Questions.push_back(newQuestion);
    return true;
}

bool ExamGUI::initGUI(Engine::UserInterface& gui)
{
    const glm::vec2 MARGIN(5.0f);
    const glm::vec2 EXAM_WINDOW_SIZE = glm::vec2(window_.width(), window_.height() - 100.0f) - 2.0f * MARGIN;

    using namespace CEGUI;

    /* Add frame window back panel */
    _examWindow = static_cast<FrameWindow*>(gui.createWidget("TaharezLook/FrameWindow",
                  glm::vec4(MARGIN, EXAM_WINDOW_SIZE), "ExamWindow"));
    if (_examWindow == nullptr)
        return false;
    _examWindow->setAlwaysOnTop(true);
    _examWindow->setRollupEnabled(false);
    _examWindow->setSizingEnabled(false);
    _examWindow->setDragMovingEnabled(false);
    _examWindow->setAlpha(1.0f);
    _examWindow->moveToBack();
    _examWindow->enable();
    _examWindow->hide();

    /* Question Text */
    glm::vec2 OFFSET(18.0f, 18.0f);
    const glm::vec2 LABEL_SIZE(EXAM_WINDOW_SIZE.x - 45.0f, 100.0f);
    _questionText = static_cast<Window*>(gui.createWidget("TaharezLook/StaticText",
                    glm::vec4(OFFSET, LABEL_SIZE), "QuestionText", _examWindow));
    if (_questionText == nullptr)
        return false;
    _questionText->setProperty("VertFormatting", "CentreAligned");
    _questionText->setProperty("HorzFormatting", "WordWrapCentreAligned");


    OFFSET += glm::vec2(0.0f, LABEL_SIZE.y + MARGIN.y);
    const glm::vec2 QUIZ_GROUP_SIZE(EXAM_WINDOW_SIZE.x - 45.0f, 200.0f);
    const glm::vec2 RADIO_BUTTON_SIZE(QUIZ_GROUP_SIZE.x - 2.0f * MARGIN.x, 30.0f);

    /* Quiz */
    {
        _quizGroup = static_cast<GroupBox*>(gui.createWidget("TaharezLook/GroupBox", glm::vec4(OFFSET,
                     QUIZ_GROUP_SIZE), "QuizGroup", _examWindow));
        if (_quizGroup == nullptr)
            return false;
        
        const glm::vec2 ANSWER_POS(10.0f, 10.0f);
        const glm::vec2 ANSWER_OFFSET = glm::vec2(0.0f, 10.0f + RADIO_BUTTON_SIZE.y);
        
        _answerRB[0] = static_cast<RadioButton*>(gui.createWidget("TaharezLook/RadioButton",
                       glm::vec4(ANSWER_POS, RADIO_BUTTON_SIZE), "AnswerRB1", _quizGroup));
        _answerRB[1] = static_cast<RadioButton*>(gui.createWidget("TaharezLook/RadioButton", 
                       glm::vec4(ANSWER_POS + ANSWER_OFFSET, RADIO_BUTTON_SIZE), "AnswerRB2", _quizGroup));
        _answerRB[2] = static_cast<RadioButton*>(gui.createWidget("TaharezLook/RadioButton",
                       glm::vec4(ANSWER_POS + 2.0f * ANSWER_OFFSET, RADIO_BUTTON_SIZE), "AnswerRB3", _quizGroup));
        _answerRB[3] = static_cast<RadioButton*>(gui.createWidget("TaharezLook/RadioButton",
                       glm::vec4(ANSWER_POS + 3.0f * ANSWER_OFFSET, RADIO_BUTTON_SIZE), "AnswerRB4", _quizGroup));

        for (auto& answer : _answerRB) {
            if (answer == nullptr)
                return false;

            answer->setGroupID(0);
        }
    }

    const glm::vec2 BUTTON_SIZE(150.0f, 28.0f);
    const glm::vec2 CANCEL_BUTTON_POS(MARGIN.x, EXAM_WINDOW_SIZE.y - MARGIN.y - 2.0f * BUTTON_SIZE.y);
    const glm::vec2 NEXT_BUTTON_POS = glm::vec2(EXAM_WINDOW_SIZE.x - BUTTON_SIZE.x - 3.0f * MARGIN.x, CANCEL_BUTTON_POS.y);
    const glm::vec2 EXAM_PROGRESS_SIZE(300.0f, BUTTON_SIZE.y);
    const glm::vec2 EXAM_PROGRESS_POS(EXAM_WINDOW_SIZE.x / 2.0f - EXAM_PROGRESS_SIZE.x / 2.0f, CANCEL_BUTTON_POS.y);

    /* Other UI */
    {
        _cancelButton = static_cast<PushButton*>(gui.createWidget("TaharezLook/Button",
                        glm::vec4(CANCEL_BUTTON_POS, BUTTON_SIZE), "CancelButton", _examWindow));
        _nextQuestionButton = static_cast<PushButton*>(gui.createWidget("TaharezLook/Button",
                              glm::vec4(NEXT_BUTTON_POS, BUTTON_SIZE), "NextButton", _examWindow));
        _examProgress = static_cast<ProgressBar*>(gui.createWidget("TaharezLook/ProgressBar",
                        glm::vec4(EXAM_PROGRESS_POS, EXAM_PROGRESS_SIZE), "ExamProgress", _examWindow));
        if (_cancelButton == nullptr || _nextQuestionButton == nullptr || _examProgress == nullptr)
            return false;

        _cancelButton->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&ExamGUI::cancelButtonClicked, this));
        _nextQuestionButton->subscribeEvent(PushButton::EventClicked, Event::Subscriber(&ExamGUI::nextButtonClicked, this));
    }

    return true;
}

void ExamGUI::setAnswerText(size_t questionIndex)
{
    for (size_t i = 0; i < 4; i++)
        _answerRB[i]->setText(_exam.Questions[questionIndex].Answers[i].Text);
}

void ExamGUI::clearSelectedAnswer()
{
    for (auto& answer : _answerRB)
        answer->setSelected(false);
}

void ExamGUI::updateTotalResult()
{
    size_t selectedAnswer;
    for (size_t i = 0; i < 4; i++) {
        if (_answerRB[i]->isSelected()) {
            selectedAnswer = i;
            break;
        }
    }

    if (_exam.Questions[_exam.CurrentQuestion].Answers[selectedAnswer].Correct == true)
        _exam.CorrectAnswers++;
}

void ExamGUI::showResults()
{
    /* If 4 or more answers are correct, this questionPlatform is marked as done. (test passed) */
    if (_exam.CorrectAnswers >= 4)
        _currentQuestion->setSolved(true);

    /* Reset Exam values */
    cancelExam();

    /* Enable gui for Results */
}

bool ExamGUI::cancelButtonClicked(const CEGUI::EventArgs& e)
{
    cancelExam();
    return true;
}

bool ExamGUI::nextButtonClicked(const CEGUI::EventArgs& e)
{
    updateTotalResult();

    /* Increment Question */
    _exam.CurrentQuestion++;
    _examProgress->setProgress((float)_exam.CurrentQuestion / 7.0f );
    if (_exam.CurrentQuestion == 7) {
        showResults();
        return true;
    }

    /* Show next Question */
    _examWindow->setText(_exam.ExamName);
    setAnswerText(_exam.CurrentQuestion);
    _questionText->setText(_exam.Questions[_exam.CurrentQuestion].Text);

    clearSelectedAnswer();
    return true;
}

#undef English
#undef Croatian