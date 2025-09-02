#include "health_assessment_widget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QFrame>
#include <QStackedWidget>
#include <QRadioButton>
#include <QButtonGroup>
#include <QSpacerItem>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QVariant>   // 如果后面有 toVariant() 之类操作就留着
#include <QDebug>
HealthAssessmentWidget::HealthAssessmentWidget(QWidget *parent)
        : QWidget(parent), mainStack(nullptr), questionnairePage(nullptr),
          resultPage(nullptr), questionLayout(nullptr), submitButton(nullptr),
          scoreLabel(nullptr), summaryLabel(nullptr), descriptionLabel(nullptr),
          suggestionLayout(nullptr), resultFrame(nullptr), currentScore(0)
{
    setObjectName("HealthAssessmentWidget");
    setupQuestions();
    setupResults();
    initUI();
    initStyleSheets();
}

HealthAssessmentWidget::~HealthAssessmentWidget() {}

void HealthAssessmentWidget::initUI() {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // 顶部标题栏
    QWidget* header = new QWidget();
    header->setObjectName("assessmentHeader");
    QHBoxLayout* headerLayout = new QHBoxLayout(header);
    headerLayout->setContentsMargins(20, 15, 20, 15);

    QPushButton *backButton = new QPushButton("← 返回首页");
    backButton->setObjectName("backButton");
    connect(backButton, &QPushButton::clicked, this, &HealthAssessmentWidget::backRequested);

    QLabel *title = new QLabel("健康自测问卷");
    title->setObjectName("pageTitle");

    QLabel *userName = new QLabel("张患者");
    userName->setObjectName("userName");

    headerLayout->addWidget(backButton);
    headerLayout->addStretch();
    headerLayout->addWidget(title);
    headerLayout->addStretch();
    headerLayout->addWidget(userName);

    mainLayout->addWidget(header);

    // 主内容区域
    mainStack = new QStackedWidget();

    // 创建问卷页面和结果页面
    questionnairePage = createQuestionnairePanel();
    resultPage = createResultPanel();

    mainStack->addWidget(questionnairePage);
    mainStack->addWidget(resultPage);

    mainLayout->addWidget(mainStack);

    // 默认显示问卷页面
    showQuestionnaire();
}

QWidget* HealthAssessmentWidget::createQuestionnairePanel() {
    QScrollArea *scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setObjectName("questionnaireScrollArea");

    QWidget *container = new QWidget();
    QVBoxLayout *containerLayout = new QVBoxLayout(container);
    containerLayout->setContentsMargins(30, 20, 30, 20);
    containerLayout->setSpacing(25);

    // 问卷主体框架
    QFrame *questionnaireFrame = new QFrame();
    questionnaireFrame->setObjectName("questionnaireFrame");

    QVBoxLayout *frameLayout = new QVBoxLayout(questionnaireFrame);
    frameLayout->setContentsMargins(40, 30, 40, 40);
    frameLayout->setSpacing(25);

    // 问卷标题和说明
    QLabel *formTitle = new QLabel("健康自测问卷");
    formTitle->setObjectName("formTitle");
    formTitle->setAlignment(Qt::AlignCenter);

    QLabel *description = new QLabel("请根据您最近一周的身体状况，回答以下问题。本问卷旨在帮助您了解自己的健康状况，结果仅供参考。");
    description->setObjectName("formDescription");
    description->setWordWrap(true);

    frameLayout->addWidget(formTitle);
    frameLayout->addWidget(description);

    // 问题列表布局
    questionLayout = new QVBoxLayout();
    questionLayout->setSpacing(30);

    // 创建所有问题
    for (int i = 0; i < questions.size(); ++i) {
        questionLayout->addWidget(createQuestionWidget(i, questions[i]));
    }

    frameLayout->addLayout(questionLayout);

    // 提交按钮
    submitButton = new QPushButton("提交评估");
    submitButton->setObjectName("submitButton");
    submitButton->setEnabled(false);
    connect(submitButton, &QPushButton::clicked, this, &HealthAssessmentWidget::onSubmitClicked);

    QHBoxLayout *submitLayout = new QHBoxLayout();
    submitLayout->addStretch();
    submitLayout->addWidget(submitButton);
    submitLayout->addStretch();

    frameLayout->addLayout(submitLayout);

    containerLayout->addWidget(questionnaireFrame);
    containerLayout->addStretch();

    scrollArea->setWidget(container);
    return scrollArea;
}

QWidget* HealthAssessmentWidget::createResultPanel() {
    QScrollArea *scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setObjectName("resultScrollArea");

    QWidget *container = new QWidget();
    QVBoxLayout *containerLayout = new QVBoxLayout(container);
    containerLayout->setContentsMargins(30, 20, 30, 20);
    containerLayout->setSpacing(25);

    // 结果主体框架
    resultFrame = new QFrame();
    resultFrame->setObjectName("resultFrame");

    QVBoxLayout *frameLayout = new QVBoxLayout(resultFrame);
    frameLayout->setContentsMargins(40, 30, 40, 40);
    frameLayout->setSpacing(25);

    // 结果标题
    QLabel *resultTitle = new QLabel("健康自测结果");
    resultTitle->setObjectName("resultTitle");
    resultTitle->setAlignment(Qt::AlignCenter);

    // 分数显示框
    QFrame *scoreFrame = new QFrame();
    scoreFrame->setObjectName("scoreFrame");

    QVBoxLayout *scoreFrameLayout = new QVBoxLayout(scoreFrame);
    scoreFrameLayout->setContentsMargins(20, 15, 20, 15);

    scoreLabel = new QLabel();
    scoreLabel->setObjectName("scoreLabel");
    scoreLabel->setAlignment(Qt::AlignCenter);

    scoreFrameLayout->addWidget(scoreLabel);

    // 结果描述
    summaryLabel = new QLabel();
    summaryLabel->setObjectName("summaryLabel");
    summaryLabel->setWordWrap(true);

    descriptionLabel = new QLabel();
    descriptionLabel->setObjectName("descriptionLabel");
    descriptionLabel->setWordWrap(true);

    // 健康建议框
    QFrame *suggestionFrame = new QFrame();
    suggestionFrame->setObjectName("suggestionFrame");

    suggestionLayout = new QVBoxLayout(suggestionFrame);
    suggestionLayout->setContentsMargins(20, 15, 20, 15);
    suggestionLayout->setSpacing(8);

    QLabel *suggestionTitle = new QLabel("健康建议：");
    suggestionTitle->setObjectName("suggestionTitle");
    suggestionLayout->addWidget(suggestionTitle);

    // 重新测试按钮
    QPushButton *retakeButton = new QPushButton("重新测试");
    retakeButton->setObjectName("retakeButton");
    connect(retakeButton, &QPushButton::clicked, this, &HealthAssessmentWidget::onRetakeClicked);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    buttonLayout->addWidget(retakeButton);
    buttonLayout->addStretch();

    frameLayout->addWidget(resultTitle);
    frameLayout->addWidget(scoreFrame);
    frameLayout->addWidget(summaryLabel);
    frameLayout->addWidget(descriptionLabel);
    frameLayout->addWidget(suggestionFrame);
    frameLayout->addLayout(buttonLayout);

    containerLayout->addWidget(resultFrame);
    containerLayout->addStretch();

    scrollArea->setWidget(container);
    return scrollArea;
}

QWidget* HealthAssessmentWidget::createQuestionWidget(int index, const Question &question) {
    QFrame *questionFrame = new QFrame();
    questionFrame->setObjectName("questionFrame");

    QVBoxLayout *layout = new QVBoxLayout(questionFrame);
    layout->setContentsMargins(20, 20, 20, 20);
    layout->setSpacing(15);

    // 问题标题
    QLabel *questionLabel = new QLabel(QString("%1. %2").arg(index + 1).arg(question.text));
    questionLabel->setObjectName("questionLabel");
    questionLabel->setWordWrap(true);
    layout->addWidget(questionLabel);

    // 选项布局
    QGridLayout *optionLayout = new QGridLayout();
    optionLayout->setSpacing(15);

    QButtonGroup *buttonGroup = new QButtonGroup(this);
    buttonGroup->setExclusive(true);
    buttonGroups.append(buttonGroup);

    connect(buttonGroup, QOverload<int>::of(&QButtonGroup::buttonClicked),
            this, &HealthAssessmentWidget::onAnswerChanged);

    for (int i = 0; i < question.options.size(); ++i) {
        QRadioButton *radioButton = new QRadioButton(question.options[i]);
        radioButton->setObjectName("optionRadioButton");

        buttonGroup->addButton(radioButton, question.scores[i]);

        int row = i / 2;
        int col = i % 2;
        optionLayout->addWidget(radioButton, row, col);
    }

    layout->addLayout(optionLayout);
    return questionFrame;
}

void HealthAssessmentWidget::setupQuestions() {
    questions = {
            {
                    "您最近的睡眠质量如何？",
                    {"很好", "一般", "较差", "很差"},
                    {3, 2, 1, 0}
            },
            {
                    "您最近的食欲如何？",
                    {"很好", "一般", "较差", "很差"},
                    {3, 2, 1, 0}
            },
            {
                    "您最近的情绪状态如何？",
                    {"积极乐观", "基本稳定", "偶尔低落", "经常焦虑"},
                    {3, 2, 1, 0}
            },
            {
                    "您最近的体力状况如何？",
                    {"精力充沛", "基本正常", "容易疲劳", "非常疲劳"},
                    {3, 2, 1, 0}
            },
            {
                    "您是否有慢性疾病症状加重？",
                    {"没有不适", "轻微不适", "明显不适", "严重不适"},
                    {3, 2, 1, 0}
            }
    };
}

void HealthAssessmentWidget::setupResults() {
    healthResults[0] = {
            "0-5分",
            "您的健康状况需要关注，建议咨询医生进行详细检查。",
            "根据您的回答，我们为您生成了以下健康评估结果：",
            {
                    "尽快咨询医生进行专业评估",
                    "建立健康的生活习惯和作息规律",
                    "注意饮食营养均衡，避免不健康食品",
                    "适当进行轻度运动，逐步改善身体状况"
            },
            "#FED7D7",  // 浅红色背景
            "#C53030"   // 深红色文字
    };

    healthResults[1] = {
            "6-10分",
            "您的健康状况一般，需要注意一些健康问题。",
            "根据您的回答，我们为您生成了以下健康评估结果：",
            {
                    "改善睡眠质量，建立规律作息",
                    "注意饮食均衡，多吃蔬菜水果",
                    "增加日常活动量，提高身体活力",
                    "学习压力管理技巧，保持心情愉悦"
            },
            "#FEFCBF",  // 浅黄色背景
            "#B7791F"   // 深黄色文字
    };

    healthResults[2] = {
            "11-15分",
            "您的健康状况良好！继续保持健康的生活习惯，定期进行健康检查。",
            "根据您的回答，我们为您生成了以下健康评估结果：",
            {
                    "保持规律作息，保证充足睡眠",
                    "均衡饮食，多吃蔬菜水果",
                    "每周保持适量运动",
                    "定期进行健康体检"
            },
            "#C6F6D5",  // 浅绿色背景
            "#2F855A"   // 深绿色文字
    };
}

void HealthAssessmentWidget::onSubmitClicked() {
    int totalScore = calculateTotalScore();
    showResults(totalScore);
}

void HealthAssessmentWidget::onRetakeClicked() {
    // 重置所有选择
    for (QButtonGroup *group : buttonGroups) {
        if (group->checkedButton()) {
            group->checkedButton()->setChecked(false);
        }
    }

    submitButton->setEnabled(false);
    showQuestionnaire();
}

void HealthAssessmentWidget::onAnswerChanged() {
    // 检查是否所有问题都已回答
    bool allAnswered = true;
    for (QButtonGroup *group : buttonGroups) {
        if (!group->checkedButton()) {
            allAnswered = false;
            break;
        }
    }

    submitButton->setEnabled(allAnswered);
}

void HealthAssessmentWidget::showQuestionnaire() {
    mainStack->setCurrentWidget(questionnairePage);
}

void HealthAssessmentWidget::showResults(int totalScore) {
    currentScore = totalScore;
    HealthResult result = getHealthResult(totalScore);

    // 更新分数显示
    scoreLabel->setText(QString("您的得分：%1 / 15").arg(totalScore));

    // 设置分数框颜色
    QString scoreFrameStyle = QString(
            "QFrame#scoreFrame { background-color: %1; border-radius: 8px; }"
            "QLabel#scoreLabel { color: %2; font-size: 18px; font-weight: bold; }"
    ).arg(result.backgroundColor, result.textColor);

    resultFrame->setStyleSheet(scoreFrameStyle);

    // 更新结果文本
    summaryLabel->setText(result.summary);
    descriptionLabel->setText(result.description);

    // 清空并更新建议列表
    QLayoutItem *item;
    while (suggestionLayout->count() > 1) {  // 保留标题
        item = suggestionLayout->takeAt(1);
        if (item->widget()) {
            delete item->widget();
        }
        delete item;
    }

    for (const QString &suggestion : result.suggestions) {
        QLabel *suggestionLabel = new QLabel("• " + suggestion);
        suggestionLabel->setObjectName("suggestionItem");
        suggestionLabel->setWordWrap(true);
        suggestionLayout->addWidget(suggestionLabel);
    }

    mainStack->setCurrentWidget(resultPage);
}

int HealthAssessmentWidget::calculateTotalScore() {
    int total = 0;
    for (QButtonGroup *group : buttonGroups) {
        if (group->checkedButton()) {
            total += group->checkedId();
        }
    }
    return total;
}

HealthResult HealthAssessmentWidget::getHealthResult(int score) {
    if (score >= 11) {
        return healthResults[2];  // 11-15分
    } else if (score >= 6) {
        return healthResults[1];  // 6-10分
    } else {
        return healthResults[0];  // 0-5分
    }
}

void HealthAssessmentWidget::initStyleSheets() {
    this->setStyleSheet(R"(
        #HealthAssessmentWidget {
            background-color: #F0F4F8;
        }

        #assessmentHeader {
            background-color: white;
            border-bottom: 1px solid #E2E8F0;
        }

        #pageTitle {
            font-size: 20px;
            font-weight: bold;
            color: #2D3748;
        }

        #backButton {
            font-size: 14px;
            color: #4A5568;
            border: none;
            background: transparent;
            padding: 8px 12px;
        }
        #backButton:hover {
            color: #2D3748;
            background-color: #F7FAFC;
            border-radius: 6px;
        }

        #userName {
            font-size: 14px;
            color: #3182CE;
            background-color: #EBF8FF;
            padding: 6px 12px;
            border-radius: 15px;
        }

        #questionnaireScrollArea, #resultScrollArea {
            background-color: transparent;
        }

        #questionnaireFrame, #resultFrame {
            background-color: white;
            border-radius: 12px;
            border: 1px solid #E2E8F0;
        }

        #formTitle, #resultTitle {
            font-size: 24px;
            font-weight: bold;
            color: #2D3748;
            margin-bottom: 10px;
        }

        #formDescription {
            font-size: 14px;
            color: #718096;
            line-height: 1.6;
        }

        #questionFrame {
            background-color: #F7FAFC;
            border-radius: 8px;
            border: 1px solid #E2E8F0;
        }

        #questionLabel {
            font-size: 16px;
            font-weight: bold;
            color: #2D3748;
        }

        #optionRadioButton {
            font-size: 14px;
            color: #4A5568;
            padding: 5px;
        }
        #optionRadioButton:checked {
            color: #3182CE;
            font-weight: bold;
        }

        #submitButton {
            background-color: #3182CE;
            color: white;
            border: none;
            border-radius: 8px;
            padding: 12px 24px;
            font-size: 16px;
            font-weight: bold;
            min-width: 120px;
        }
        #submitButton:hover {
            background-color: #2B6CB0;
        }
        #submitButton:disabled {
            background-color: #A0AEC0;
            cursor: not-allowed;
        }

        #scoreFrame {
            border-radius: 8px;
            margin: 10px 0;
        }

        #scoreLabel {
            font-size: 18px;
            font-weight: bold;
        }

        #summaryLabel {
            font-size: 16px;
            color: #2D3748;
            margin: 15px 0;
        }

        #descriptionLabel {
            font-size: 14px;
            color: #4A5568;
            margin-bottom: 20px;
        }

        #suggestionFrame {
            background-color: #F7FAFC;
            border-radius: 8px;
            border-left: 4px solid #3182CE;
        }

        #suggestionTitle {
            font-size: 16px;
            font-weight: bold;
            color: #3182CE;
        }

        #suggestionItem {
            font-size: 14px;
            color: #4A5568;
            line-height: 1.5;
            margin: 2px 0;
        }

        #retakeButton {
            background-color: #E2E8F0;
            color: #4A5568;
            border: none;
            border-radius: 8px;
            padding: 10px 20px;
            font-size: 14px;
            min-width: 100px;
        }
        #retakeButton:hover {
            background-color: #CBD5E0;
        }
    )");
}
