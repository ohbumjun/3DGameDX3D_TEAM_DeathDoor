
#include "BehaviorTreeWindow.h"
#include "IMGUIManager.h"
#include "IMGUIButton.h"
#include "../DeathDoor/Component/GameStateComponent.h"
#include "Component/Node/SequenceNode.h"
#include "Component/Node/SelectorNode.h"
#include "Component/Node/ActionNode.h"
#include "Component/Node/ConditionNode.h"

// TODO : Death Door Behavior Tree Node 추가될 때마다 업데이트
#include "../DeathDoor/Component/Node/MoveNode.h"
#include "../DeathDoor/Component/Node/IdleNode.h"
#include "../DeathDoor/Component/Node/MoveInputCheckNode.h"
#include "../DeathDoor/Component/Node/NoInterruptNode.h"
#include "../DeathDoor/Component/Node/NormalAttack.h"
#include "../DeathDoor/Component/Node/MouseLButtonCheckNode.h"
#include "../DeathDoor/Component/Node/MouseRButtonCheckNode.h"
#include "../DeathDoor/Component/Node/MouseRButtonUpCheckNode.h"
#include "../DeathDoor/Component/Node/RotateAttackDirectionNode.h"
#include "../DeathDoor/Component/Node/NegateNode.h"
#include "../DeathDoor/Component/Node/DeathNode.h"
#include "../DeathDoor/Component/Node/ReadyToShoot.h"
#include "../DeathDoor/Component/Node/ShootNode.h"
#include "../DeathDoor/Component/Node/CancleShootNode.h"
#include "../DeathDoor/Component/Node/AddFallingFloorCallbackNode.h"
#include "../DeathDoor/Component/Node/Lockstone3TriggerBoxHitCheck.h"
#include "../DeathDoor/Component/Node/Lockstone3TriggerBoxAction.h"
#include "../DeathDoor/Component/Node/CheckDetectRangeNode.h"
#include "../DeathDoor/Component/Node/FindPathNode.h"
#include "../DeathDoor/Component/Node/ClearPathListNode.h"
#include "../DeathDoor/Component/Node/PlayerEnterZoneCheckNode.h"
#include "../DeathDoor/Component/Node/PlayerEnterTriggerIsEnable.h"
#include "../DeathDoor/Component/Node/MeleeRangeCheckNode.h"
#include "../DeathDoor/Component/Node/IsCombatCheck.h"
#include "../DeathDoor/Component/Node/DeathCheck.h"
#include "../DeathDoor/Component/Node/PostAttackDelayCheck.h"
#include "../DeathDoor/Component/Node/AttackCoolTimeCheck.h"
#include "../DeathDoor/Component/Node/PathFindExecuteCheck.h"
#include "../DeathDoor/Component/Node/PathFindEnableCheck.h"
#include "../DeathDoor/Component/Node/PathListEmptyCheck.h"

#include "../DeathDoor/Component/Node/ClimbDown.h"
#include "../DeathDoor/Component/Node/ClimbDownEnd.h"
#include "../DeathDoor/Component/Node/ClimbDownStart.h"
#include "../DeathDoor/Component/Node/ClimbUp.h"
#include "../DeathDoor/Component/Node/ClimbUpStart.h"
#include "../DeathDoor/Component/Node/ClimbUpEnd.h"
#include "../DeathDoor/Component/Node/SKeyInputCheck.h"
#include "../DeathDoor/Component/Node/WKeyInputCheck.h"
#include "../DeathDoor/Component/Node/ClimbDownContinueCheck.h"
#include "../DeathDoor/Component/Node/ClimbDownEndCheck.h"
#include "../DeathDoor/Component/Node/ClimbDownStartCheck.h"
#include "../DeathDoor/Component/Node/ClimbUpContinueCheck.h"
#include "../DeathDoor/Component/Node/ClimbingUpStartCheck.h"
#include "../DeathDoor/Component/Node/ClimbUpEndCheck.h"
#include "../DeathDoor/Component/Node/ClimbKeyEnableCheck.h"
#include "../DeathDoor/Component/Node/ClimbPause.h"
#include "../DeathDoor/Component/Node/IsClimbingCheck.h"
#include "../DeathDoor/Component/Node/ClimbPause.h"
#include "../DeathDoor/Component/Node/ClimbKeyEnableCheck.h"

#include "../DeathDoor/Component/Node/HitCheckNode.h"
#include "../DeathDoor/Component/Node/HitBackNode.h"
#include "../DeathDoor/Component/Node/RollInputCheck.h"
#include "../DeathDoor/Component/Node/PlayerRoll.h"
#include "../DeathDoor/Component/Node/UpdateInputQueue.h"
#include "../DeathDoor/Component/Node/PathFindExecuteCheck.h"
#include "../DeathDoor/Component/Node/StraightPathCheck.h"
#include "../DeathDoor/Component/Node/ChasePlayerNode.h"
#include "../DeathDoor/Component/Node/MonsterHitBack.h"
#include "../DeathDoor/Component/Node/MeleeAttackNode.h"
#include "../DeathDoor/Component/Node/PathListEmptyCheck.h"

// BossKnight
#include "../DeathDoor/Component/Node/BossKnightContinueAttackNode.h"
#include "../DeathDoor/Component/Node/BossKnightCutScenePlayCheck.h"
#include "../DeathDoor/Component/Node/BossKnightCutScenePlayNode.h"
#include "../DeathDoor/Component/Node/BossKnightFinalAttackCheck.h"
#include "../DeathDoor/Component/Node/BossKnightFinalAttackNode.h"
#include "../DeathDoor/Component/Node/BossKnightIdleNode.h"
#include "../DeathDoor/Component/Node/BossKnightJumpAttackNode.h"
#include "../DeathDoor/Component/Node/BossKnightJumpAttackRangeCheck.h"
#include "../DeathDoor/Component/Node/BossKnightMeleeAttackNode.h"
#include "../DeathDoor/Component/Node/BossKnightPlayerEnterZoneCheck.h"
#include "../DeathDoor/Component/Node/BossKnightSlamEnd.h"
#include "../DeathDoor/Component/Node/BossKnightWalkNode.h"
#include "../DeathDoor/Component/Node/BossKnightCutScenePlayNode.h"
#include "../DeathDoor/Component/Node/BossKnightContinueAttackCheck.h"

// BossBetty
#include "../DeathDoor/Component/Node/BossBettyAngryAttackNode.h"
#include "../DeathDoor/Component/Node/BossBettyChangeAttackDirNode.h"
#include "../DeathDoor/Component/Node/BossBettyCloseAttackNode.h"
#include "../DeathDoor/Component/Node/BossBettyJumpAttackNode.h"
#include "../DeathDoor/Component/Node/BossBettySpinAttackNode.h"
#include "../DeathDoor/Component/Node/BossBettyThrowNode.h"
#include "../DeathDoor/Component/Node/BossBettyIntroNode.h"
#include "../DeathDoor/Component/Node/BossBettyIdleBeastNode.h"

#include "../DeathDoor/Component/Node/BossBettyChangeDirCheck.h"
#include "../DeathDoor/Component/Node/BossBettyFarAttackTypeCheck.h"
#include "../DeathDoor/Component/Node/BossBettyHPStateCheck.h"
#include "../DeathDoor/Component/Node/BossBettyThrowAttackCheck.h"
#include "../DeathDoor/Component/Node/BossBettyIdleAnimCheck.h"
#include "../DeathDoor/Component/Node/BossBettyIntroAnimCheck.h"

// CrowBoss
#include "../DeathDoor/Component/Node/CrowBossCutScenePlayCheck.h"
#include "../DeathDoor/Component/Node/CrowBossCutScenePlayNode.h"
#include "../DeathDoor/Component/Node/CrowBossRunNode.h"
#include "../DeathDoor/Component/Node/CrowBossSpinNode.h"
#include "../DeathDoor/Component/Node/CrowBossPhasePickNode.h"
#include "../DeathDoor/Component/Node/CrowBossBypassCheck.h"
#include "../DeathDoor/Component/Node/CrowBossJumpStartCheck.h"
#include "../DeathDoor/Component/Node/CrowBossLandingCheck.h"
#include "../DeathDoor/Component/Node/CrowBossJump.h"
#include "../DeathDoor/Component/Node/CrowBossJumpAttackRangeCheck.h"
#include "../DeathDoor/Component/Node/CrowBossDoneCheck.h"
#include "../DeathDoor/Component/Node/CrowBossFirstShootCheck.h"
#include "../DeathDoor/Component/Node/CrowBossShootCheck.h"
#include "../DeathDoor/Component/Node/CrowBossShootEndCheck.h"
#include "../DeathDoor/Component/Node/CrowBossShootReadyCheck.h"
#include "../DeathDoor/Component/Node/CrowBossFlyNode.h"
#include "../DeathDoor/Component/Node/CrowBossMoveShootSpot.h"
#include "../DeathDoor/Component/Node/CrowBossResetShootState.h"
#include "../DeathDoor/Component/Node/CrowBossShootNode.h"
#include "../DeathDoor/Component/Node/CrowBossSlide.h"
#include "../DeathDoor/Component/Node/CrowBossSpitting.h"

// SporeBoomer
#include "../DeathDoor/Component/Node/SporeBoomerShootNode.h"

// HeadRoller
#include "../DeathDoor/Component/Node/HeadRollerStunCheck.h"
#include "../DeathDoor/Component/Node/HeadRollerRollNode.h"
#include "../DeathDoor/Component/Node/HeadRollerStunNode.h"

// Dodger
#include "../DeathDoor/Component/Node/DodgerDashCheck.h"
#include "../DeathDoor/Component/Node/DodgerDashNode.h"
#include "../DeathDoor/Component/Node/DodgerFirstAttackNode.h"
#include "../DeathDoor/Component/Node/DodgerPostAttackNode.h"
#include "../DeathDoor/Component/Node/DodgerSecondAttackCheck.h"
#include "../DeathDoor/Component/Node/DodgerSecondAttackNode.h"

// Plauge Knight
#include "../DeathDoor/Component/Node/PlagueKnightShootNode.h"

// Bat
#include "../DeathDoor/Component/Node/BatRecognizeStartCheck.h"
#include "../DeathDoor/Component/Node/BatRecognizeEndCheck.h"
#include "../DeathDoor/Component/Node/BatRecognize.h"

// Lurker
#include "../DeathDoor/Component/Node/LurkerHopStartCheck.h"
#include "../DeathDoor/Component/Node/LurkerHop.h"
#include "../DeathDoor/Component/Node/LurkerMeleeAttackPrep.h"

// Grunt Common
#include "../DeathDoor/Component/Node/GruntCommonHitNode.h"
#include "../DeathDoor/Component/Node/GruntCommonTraceNode.h"
#include "../DeathDoor/Component/Node/GruntCommonWalkNode.h"
#include "../DeathDoor/Component/Node/GruntCommonAttackNode.h"
#include "../DeathDoor/Component/Node/GruntCommonSpawnNode.h"

#include "../DeathDoor/Component/Node/GruntCommonSpawnCheckNode.h"
#include "../DeathDoor/Component/Node/GruntCommonMoveStartCheckNode.h"

#include "ObjectComponentWindow.h"
#include "ObjectHierarchyWindow.h"
#include "../EditorInfo.h"
#include "../EditorUtil.h"
#include "../DeathDoor/DDUtil.h"

CBehaviorTreeWindow::CBehaviorTreeWindow()  :
    m_Select(false),
    m_TypeSelectIndex(-1),
    m_ActionSelectIndex(-1),
    m_StateComponent(nullptr)
{
    m_PrevViewName = "Select Node Type";
    m_LeafNodePrevViewName = "Select Node Action";

    m_vecNodeType.push_back("Sequence Node");
    m_vecNodeType.push_back("Selector Node");
    m_vecNodeType.push_back("Action Node");
    m_vecNodeType.push_back("Condition Node");
    m_vecNodeType.push_back("Decorator Node");

}

CBehaviorTreeWindow::~CBehaviorTreeWindow()
{
    if (m_Delegate.m_IsCleared)
        return;

    size_t Count = m_Delegate.GetTemplateCount();

    for (size_t i = 0; i < Count; ++i)
    {
        GraphEditor::Template tmp = m_Delegate.GetTemplate(i);
        if (tmp.mInputNames)
        {
            delete[] tmp.mInputNames;
        }

        if (tmp.mOutputNames)
        {
            delete[] tmp.mOutputNames;
        }
    }

   m_Delegate.m_IsCleared = true;
}

bool CBehaviorTreeWindow::Init()
{
	CIMGUIWindow::Init();

    if (m_StateComponent)
        m_StateComponent->SetTreeUpdate(false);

	return true;
}

void CBehaviorTreeWindow::Update(float DeltaTime)
{
    if (!m_Open)
        return;

    CObjectComponentWindow* ObjCompWindow = (CObjectComponentWindow*)CIMGUIManager::GetInst()->FindIMGUIWindow(OBJECTCOMPONENT_LIST);
    CObjectHierarchyWindow* ObjWindow = (CObjectHierarchyWindow*)CIMGUIManager::GetInst()->FindIMGUIWindow(OBJECT_HIERARCHY);


    if (ObjWindow && ObjCompWindow)
    {
        CGameObject* SelectObj = ObjWindow->GetSelectObject();

        // OBJ가 추가
        if (SelectObj)
        {
            CGameStateComponent* State = SelectObj->FindComponentFromType<CGameStateComponent>();

            if (State)
            {
                // 만약 Object가 Load되고 처음 Behavior Tree를 열었다면 State Component가 갖고 있는 Behavior Tree안에 노드들이 Graph Editor에 하나도 나오지 않을테니
                // Behavior Tree안에 노드들을 Graph Editor에 출력되게 해야한다
                if (State->GetBehaviorTree()->GetRootNode() && m_Delegate.GetNodeCount() == 0)
                {
                    UpdateLoadNode((CCompositeNode*)(State->GetBehaviorTree()->GetRootNode()));
                }
            }
        }
    }


    GraphEditor::FitOnScreen fit = GraphEditor::FitOnScreen::Fit_None;

    ImGui::Begin("Graph Editor", NULL, 0);

    if (ImGui::Button("Fit all nodes"))
    {
        fit = GraphEditor::FitOnScreen::Fit_AllNodes;
    }

    ImGui::SameLine();

    if (ImGui::Button("Fit selected nodes"))
    {
        fit = GraphEditor::FitOnScreen::Fit_SelectedNodes;
    }


    ImGui::PushID(m_WidgetID);
    ImGui::PushItemWidth(150.f);

    // 노드 타입 결정하는 콤보
    if (ImGui::BeginCombo("", m_PrevViewName.c_str(), m_ComboFlag))
    {
        size_t  ItemCount = m_vecNodeType.size();

        for (size_t i = 0; i < ItemCount; ++i)
        {
            m_Select = m_TypeSelectIndex == i;

            if (ImGui::Selectable(m_vecNodeType[i].c_str(), m_Select))
            {
                //if (m_SelectIndex != i && m_SelectCallback)
                //    m_SelectCallback((int)i, m_vecItem[i].c_str());

                m_PrevViewName = m_vecNodeType[i];

                m_TypeSelectIndex = (int)i;
            }

            if (m_Select)
                ImGui::SetItemDefaultFocus();
        }

        ImGui::EndCombo();
    }

    ImGui::PopID();

    ImGui::SameLine();


    // 노드가 ActionNode이거나 Condition Node일 때 Invoke할 동작을 정하고 만들어야한다
    if (m_TypeSelectIndex == 2 || m_TypeSelectIndex == 3 || m_TypeSelectIndex == 4)
    {
        m_vecNodeAction.clear();

        if (m_TypeSelectIndex == 2)
        {
            PrintActionNodes();
        }

        else if (m_TypeSelectIndex == 3)
        {
            PrintConditionNodes();
        }

        else if (m_TypeSelectIndex == 4)
        {
            m_vecNodeAction.push_back("Negate");
        }

        ImGui::PushID(m_WidgetID + 1);
        ImGui::PushItemWidth(150.f);

        if (ImGui::BeginCombo("", m_LeafNodePrevViewName.c_str(), m_ComboFlag))
        {
            size_t  ItemCount = m_vecNodeAction.size();

            for (size_t i = 0; i < ItemCount; ++i)
            {
                m_Select = m_ActionSelectIndex == i;

                if (ImGui::Selectable(m_vecNodeAction[i].c_str(), m_Select))
                {
                    //if (m_SelectIndex != i && m_SelectCallback)
                    //    m_SelectCallback((int)i, m_vecItem[i].c_str());

                    m_LeafNodePrevViewName = m_vecNodeAction[i];

                    m_ActionSelectIndex = (int)i;
                }

                if (m_Select)
                    ImGui::SetItemDefaultFocus();
            }

            ImGui::EndCombo();
        }

        ImGui::PopID();

        ImGui::SameLine();
    }

    // 노드 이름
    bool	Input = false;

    ImGui::PushID(m_WidgetID + 1);
    ImGui::PushItemWidth(100.f);

    char HintText[1024] = "Enter Node Name";
    char NameInput[256] = "";

    Input = ImGui::InputTextWithHint("##", HintText, m_Text, 1024, m_TextInputFlag);

    std::string NameInputStr = m_Text;

    if (Input)
    {
        memset(m_wText, 0, sizeof(wchar_t) * 1024);
        memset(m_TextUTF8, 0, sizeof(char) * 1024);

        int	Length = MultiByteToWideChar(CP_ACP, 0, m_Text, -1, 0, 0);
        MultiByteToWideChar(CP_ACP, 0, m_Text, -1, m_wText, Length);

        // UTF8로 변환한다.
        Length = WideCharToMultiByte(CP_UTF8, 0, m_wText, -1, 0, 0, 0, 0);
        WideCharToMultiByte(CP_UTF8, 0, m_wText, -1, m_TextUTF8, Length, 0, 0);

        if (NameInputStr.empty())
        {
            m_InputNodeName = m_TextUTF8;
        }
    }

    ImGui::PopID();

    ImGui::SameLine();

    // 노드 추가 버튼
    if (ImGui::Button("Add Node"))
    {
        OnAddNodeButton(m_Text, m_TypeSelectIndex, m_ActionSelectIndex);
    }

    ImGui::SameLine();

    if (ImGui::Button("Delete Node"))
    {
        GraphEditorDelegate::Node node;
        bool Result = m_Delegate.GetSelectedDelegateNode(node);
        if (Result)
        {
            OnDeleteNodeButton(node.name);
        }
    }

    ImGui::SameLine();

    ImGui::Dummy(ImVec2(100.f, 20.f));

    ImGui::SameLine();

    if (ImGui::Button("Run"))
    {
        if (m_StateComponent)
            m_StateComponent->SetTreeUpdate(true);
    }

    ImGui::SameLine();

    if (ImGui::Button("Stop"))
    {
        if (m_StateComponent)
            m_StateComponent->SetTreeUpdate(false);
    }

    ImGui::SameLine();

    if (ImGui::Button("Close"))
    {
        m_Open = false;
        ImGui::End();
        return;
    }

    GraphEditor::Show(m_Delegate, m_Option, m_ViewState, true, &fit);

    size_t Count = m_Delegate.mNodes.size();

    for (size_t i = 0; i < Count; ++i)
    {
        Vector2 InWindowPos;
        InWindowPos.x = m_Delegate.mNodes[i].x;
        InWindowPos.y = m_Delegate.mNodes[i].y;

        m_Delegate.mNodes[i].BehaviorTreeNode->SetInWindowPos(InWindowPos);
    }

    ImGui::End();


    //CIMGUIWindow::Update(DeltaTime);
}

void CBehaviorTreeWindow::SetStateComponent(CStateComponent* Com)
{
    m_StateComponent = Com;
}

CStateComponent* CBehaviorTreeWindow::GetStateComponent() const
{
    return m_StateComponent;
}

void CBehaviorTreeWindow::OnAddNodeButton(const char* Name, int TypeIndex, int ActionIndex)
{
    GraphEditor::TemplateIndex idx = TypeIndex;

    char NodeName[256] = {};
    strcpy_s(NodeName, Name);

    CNode* NewTreeNode = nullptr;

    CNode* ExistNode = m_StateComponent->GetBehaviorTree()->FindNode(Name);

    char NewName[256] = {};
    strcpy_s(NewName, Name);

    if (ExistNode)
        strcat_s(NewName, "_");


    switch (TypeIndex)
    {
    case 0:
        NewTreeNode = m_StateComponent->CreateTreeNode<CSequenceNode>(NewName);
        break;
    case 1:
        NewTreeNode = m_StateComponent->CreateTreeNode<CSelectorNode>(NewName);
        break;
    case 2:
    {
        // TODO : Action Node 종류 추가될 때 마다 추가
        enum ActionNode NodeActionClass;
        NodeActionClass = static_cast<ActionNode>(ActionIndex);

        // switch (NodeActionClass)
        // {
        // }

        if (NewTreeNode)
        {
            break;
        }

		// 위에서 생성되지 않았다면 클라이언트 단에서 정의된 노드임
		// TODO : Death Door Action Node 추가될 때마다 업데이트
        int DDActionIdx = ActionIndex - (int)ActionNode::ActionNodeMax;
        DDActionNode ActionNodeType = (DDActionNode)DDActionIdx;
        switch (ActionNodeType)
        {
        case DDActionNode::Move:
        {
			NewTreeNode = m_StateComponent->CreateTreeNode<CMoveNode>(Name);
			CNavAgent* Agent = m_StateComponent->GetGameObject()->FindObjectComponentFromType<CNavAgent>();

			if (Agent)
				((CMoveNode*)NewTreeNode)->SetNavAgent(Agent);
            break;
        }
        case DDActionNode::Idle:
        {
            NewTreeNode = m_StateComponent->CreateTreeNode<CIdleNode>(Name);
            break;
        }
        case DDActionNode::NormalAttack:
        {
            NewTreeNode = m_StateComponent->CreateTreeNode<CNormalAttack>(Name);
            break;
        }
        case DDActionNode::Death:
        {
            NewTreeNode = m_StateComponent->CreateTreeNode<CDeathNode>(Name);
            break;
        }
        case DDActionNode::RotateAttackDirection:
        {
            NewTreeNode = m_StateComponent->CreateTreeNode<CRotateAttackDirectionNode>(Name);
            break;
        }
        case DDActionNode::ReadyToShoot:
        {
            NewTreeNode = m_StateComponent->CreateTreeNode<CReadyToShoot>(Name);
            break;
        }
        case DDActionNode::ShootNode:
        {
            NewTreeNode = m_StateComponent->CreateTreeNode<CShootNode>(Name);
            break;
        }
        case DDActionNode::CancleShootNode:
        {
            NewTreeNode = m_StateComponent->CreateTreeNode<CCancleShootNode>(Name);
            break;
        }
        case DDActionNode::AddFallingFloorCallback:
        {
            NewTreeNode = m_StateComponent->CreateTreeNode<CAddFallingFloorCallbackNode>(Name);
            break;
        }
        case DDActionNode::Lockstone3TriggerBoxAction:
        {
            NewTreeNode = m_StateComponent->CreateTreeNode<CLockstone3TriggerBoxAction>(Name);
            break;
        }
        case DDActionNode::FindPath:
        {
            NewTreeNode = m_StateComponent->CreateTreeNode<CFindPathNode>(Name);
            break;
        }
        case DDActionNode::ClearPathList:
        {
            NewTreeNode = m_StateComponent->CreateTreeNode<CClearPathListNode>(Name);
            break;
        }
        case DDActionNode::HitBack:
        {
            NewTreeNode = m_StateComponent->CreateTreeNode<CHitBackNode>(Name);
            break;
        }
        case DDActionNode::PlayerRoll:
        {
            NewTreeNode = m_StateComponent->CreateTreeNode<CPlayerRoll>(Name);
            break;
        }
        case DDActionNode::ClimbDown:
        {
            NewTreeNode = m_StateComponent->CreateTreeNode<CClimbDown>(Name);
            break;
        }
        case DDActionNode::ClimbDownEnd:
        {
            NewTreeNode = m_StateComponent->CreateTreeNode<CClimbDownEnd>(Name);
            break;
        }        
        case DDActionNode::ClimbDownStart:
        {
            NewTreeNode = m_StateComponent->CreateTreeNode<CClimbDownStart>(Name);
            break;
        }        
        case DDActionNode::ClimbUp:
        {
            NewTreeNode = m_StateComponent->CreateTreeNode<CClimbUp>(Name);
            break;
        }        
        case DDActionNode::ClimbUpStart:
        {
            NewTreeNode = m_StateComponent->CreateTreeNode<CClimbUpStart>(Name);
            break;
        }        
        case DDActionNode::ClimbUpEnd:
        {
            NewTreeNode = m_StateComponent->CreateTreeNode<CClimbUpEnd>(Name);
            break;
        }
        case DDActionNode::ClimbPause:
        {
            NewTreeNode = m_StateComponent->CreateTreeNode<CClimbPause>(Name);
            break;
        }
        case DDActionNode::ChasePlayer:
        {
            NewTreeNode = m_StateComponent->CreateTreeNode<CChasePlayerNode>(Name);
            break;
        }
        case DDActionNode::MonsterHitBack:
        {
            NewTreeNode = m_StateComponent->CreateTreeNode<CMonsterHitBack>(Name);
            break;
        }
        case DDActionNode::MeleeAttack:
        {
            NewTreeNode = m_StateComponent->CreateTreeNode<CMeleeAttackNode>(Name);
            break;
        }

        // Boss Knight
        case DDActionNode::BossKnightContinueAttack:
        {
            NewTreeNode = m_StateComponent->CreateTreeNode<CBossKnightContinueAttackNode>(Name);
            break;
        }
        case DDActionNode::BossKnightFinalAttack:
        {
            NewTreeNode = m_StateComponent->CreateTreeNode<CBossKnightFinalAttackNode>(Name);
            break;
        }
        case DDActionNode::BossKnightJumpAttack:
        {
            NewTreeNode = m_StateComponent->CreateTreeNode<CBossKnightJumpAttackNode>(Name);
            break;
        }
        case DDActionNode::BossKnightMeleeAttack:
        {
            NewTreeNode = m_StateComponent->CreateTreeNode<CBossKnightMeleeAttackNode>(Name);
            break;
        }
        case DDActionNode::BossKnightSlamEnd:
        {
            NewTreeNode = m_StateComponent->CreateTreeNode<CBossKnightSlamEnd>(Name);
            break;
        }
        case DDActionNode::BossKnightWalk:
        {
            NewTreeNode = m_StateComponent->CreateTreeNode<CBossKnightWalkNode>(Name);
            break;
        }
        case DDActionNode::BossKnightCutScenePlay:
        {
            NewTreeNode = m_StateComponent->CreateTreeNode<CBossKnightCutScenePlayNode>(Name);
            break;
        }
        case DDActionNode::BossKnightIdle:
        {
            NewTreeNode = m_StateComponent->CreateTreeNode<CBossKnightIdleNode>(Name);
            break;
        }
        // BossBetty
        case DDActionNode::BossBettyAngryAttack:
        {
            NewTreeNode = m_StateComponent->CreateTreeNode<CBossBettyAngryAttackNode>(Name);
            break;
        }
        case DDActionNode::BossBettyChangeAttackDir:
        {
            NewTreeNode = m_StateComponent->CreateTreeNode<CBossBettyChangeAttackDirNode>(Name);
            break;
        }
        case DDActionNode::BossBettyCloseAttack:
        {
            NewTreeNode = m_StateComponent->CreateTreeNode<CBossBettyCloseAttackNode>(Name);
            break;
        }
        case DDActionNode::BossBettyJumpAttack:
        {
            NewTreeNode = m_StateComponent->CreateTreeNode<CBossBettyJumpAttackNode>(Name);
            break;
        }
        case DDActionNode::BossBettySpinAttack:
        {
            NewTreeNode = m_StateComponent->CreateTreeNode<CBossBettySpinAttackNode>(Name);
            break;
        }
        case DDActionNode::BossBettyThrowAttack:
        {
            NewTreeNode = m_StateComponent->CreateTreeNode<CBossBettyThrowNode>(Name);
            break;
        }
        case DDActionNode::BossBettyIntro:
        {
            NewTreeNode = m_StateComponent->CreateTreeNode<CBossBettyIntroNode>(Name);
            break;
        }
        case DDActionNode::BossBettyIdleBeast:
        {
            NewTreeNode = m_StateComponent->CreateTreeNode<CBossBettyIdleBeastNode>(Name);
            break;
        }
        // Crow Boss
        case DDActionNode::CrowBossCutScenePlayNode:
        {
            NewTreeNode = m_StateComponent->CreateTreeNode<CCrowBossCutScenePlayNode>(Name);
            break;
        }
        case DDActionNode::CrowBossRunNode:
        {
            NewTreeNode = m_StateComponent->CreateTreeNode<CCrowBossRunNode>(Name);
            break;
        }
        case DDActionNode::CrowBossSpinNode:
        {
            NewTreeNode = m_StateComponent->CreateTreeNode<CCrowBossSpinNode>(Name);
            break;
        }
        case DDActionNode::CrowBossJump:
        {
            NewTreeNode = m_StateComponent->CreateTreeNode<CCrowBossJump>(Name);
            break;
        }
        case DDActionNode::CrowBossFly:
        {
            NewTreeNode = m_StateComponent->CreateTreeNode<CCrowBossFlyNode>(Name);
            break;
        }
        case DDActionNode::CrowBossMoveShootSpot:
        {
            NewTreeNode = m_StateComponent->CreateTreeNode<CCrowBossMoveShootSpot>(Name);
            break;
        }
        case DDActionNode::CrowBossResetShootState:
        {
            NewTreeNode = m_StateComponent->CreateTreeNode<CCrowBossResetShootState>(Name);
            break;
        }
        case DDActionNode::CrowBossShoot:
        {
            NewTreeNode = m_StateComponent->CreateTreeNode<CCrowBossShootNode>(Name);
            break;
        }
        case DDActionNode::CrowBossSlide:
        {
            NewTreeNode = m_StateComponent->CreateTreeNode<CCrowBossSlide>(Name);
            break;
        }
        case DDActionNode::CrowBossSpitting:
        {
            NewTreeNode = m_StateComponent->CreateTreeNode<CCrowBossSpitting>(Name);
            break;
        }

        // Spore Boomer
        case DDActionNode::SporeBoomerShoot:
        {
            NewTreeNode = m_StateComponent->CreateTreeNode<CSporeBoomerShootNode>(Name);
            break;
        }

        // Head Roller
        case DDActionNode::HeadRollerRoll:
        {
            NewTreeNode = m_StateComponent->CreateTreeNode<CHeadRollerRollNode>(Name);
            break;
        }
        case DDActionNode::HeadRollerStun:
        {
            NewTreeNode = m_StateComponent->CreateTreeNode<CHeadRollerStunNode>(Name);
            break;
        }

        // Dodger
        case DDActionNode::DodgerDash:
        {
            NewTreeNode = m_StateComponent->CreateTreeNode<CDodgerDashNode>(Name);
            break;
        }
        case DDActionNode::DodgerFirstAttack:
        {
            NewTreeNode = m_StateComponent->CreateTreeNode<CDodgerFirstAttackNode>(Name);
            break;
        }
        case DDActionNode::DodgerSecondAttack:
        {
            NewTreeNode = m_StateComponent->CreateTreeNode<CDodgerSecondAttackNode>(Name);
            break;
        }
        case DDActionNode::DodgerPostAttack:
        {
            NewTreeNode = m_StateComponent->CreateTreeNode<CDodgerPostAttackNode>(Name);
            break;
        }

        // Plauge Knight
        case DDActionNode::PlagueKnightShoot:
        {
            NewTreeNode = m_StateComponent->CreateTreeNode<CPlagueKnightShootNode>(Name);
            break;
        }

        // Bat
        case DDActionNode::BatRecognize:
        {
            NewTreeNode = m_StateComponent->CreateTreeNode<CBatRecognize>(Name);
            break;
        }

        // Lurker
        case DDActionNode::LurkerHop:
        {
            NewTreeNode = m_StateComponent->CreateTreeNode<CLurkerHop>(Name);
            break;
        }
        case DDActionNode::LurkerMeleeAttackPrep:
        {
            NewTreeNode = m_StateComponent->CreateTreeNode<CLurkerMeleeAttackPrep>(Name);
            break;
        }

        // Grunt Common
        case DDActionNode::GruntCommonAttack:
        {
            NewTreeNode = m_StateComponent->CreateTreeNode<CGruntCommonAttackNode>(Name);
            break;
        }
        case DDActionNode::GruntCommonHit:
        {
            NewTreeNode = m_StateComponent->CreateTreeNode<CGruntCommonHitNode>(Name);
            break;
        }
        case DDActionNode::GruntCommonTrace:
        {
            NewTreeNode = m_StateComponent->CreateTreeNode<CGruntCommonTraceNode>(Name);
            break;
        }
        case DDActionNode::GruntCommonWalk:
        {
            NewTreeNode = m_StateComponent->CreateTreeNode<CGruntCommonWalkNode>(Name);
            break;
        }
        case DDActionNode::GruntCommonSpawn:
        {
            NewTreeNode = m_StateComponent->CreateTreeNode<CGruntCommonSpawnNode>(Name);
            break;
        }
        

		}
        break;
    }

    case 3:
    {
        // TODO : Condition Node 종류 추가될 때 마다 추가
        enum ConditionNode NodeConditionlass;
        NodeConditionlass = static_cast<ConditionNode>(ActionIndex);

        switch (NodeConditionlass)
        {
        }

        if (NewTreeNode)
        {
            break;
        }

		// 위에서 생성되지 않았다면 클라이언트 단에서 정의된 노드임
		// TODO : Death Door Condition Node 추가될 때마다 업데이트
        int DDConditionIdx = ActionIndex - (int)ConditionNode::ConditionNodeMax;
        DDConditionNode ConditionNodeType = (DDConditionNode)DDConditionIdx;

        switch (ConditionNodeType)
        {
        case DDConditionNode::MoveInputCheckNode:
            NewTreeNode = m_StateComponent->CreateTreeNode<CMoveInputCheckNode>(Name);
            break;
        case DDConditionNode::MouseLButtonCheckNode:
            NewTreeNode = m_StateComponent->CreateTreeNode<CMouseLButtonCheckNode>(Name);
            break;
        case DDConditionNode::NoInterruptNode:
            NewTreeNode = m_StateComponent->CreateTreeNode<CNoInterruptNode>(Name);
            break;
        case DDConditionNode::MouseRButtonCheckNode:
            NewTreeNode = m_StateComponent->CreateTreeNode<CMouseRButtonCheckNode>(Name);
            break;
        case DDConditionNode::MouseRButtonUpCheckNode:
            NewTreeNode = m_StateComponent->CreateTreeNode<CMouseRButtonUpCheckNode>(Name);
            break;
        case DDConditionNode::IsClimbingCheck:
            NewTreeNode = m_StateComponent->CreateTreeNode<CIsClimbingCheck>(Name);
            break;
        case DDConditionNode::SKeyInputCheck:
            NewTreeNode = m_StateComponent->CreateTreeNode<CSKeyInputCheck>(Name);
            break;
        case DDConditionNode::WKeyInputCheck:
            NewTreeNode = m_StateComponent->CreateTreeNode<CWKeyInputCheck>(Name);
            break;
        case DDConditionNode::ClimbDownContinueCheck:
            NewTreeNode = m_StateComponent->CreateTreeNode<CClimbDownContinueCheck>(Name);
            break;
        case DDConditionNode::ClimbDownEndCheck:
            NewTreeNode = m_StateComponent->CreateTreeNode<CClimbDownEndCheck>(Name);
            break;
        case DDConditionNode::ClimbDownStartCheck:
            NewTreeNode = m_StateComponent->CreateTreeNode<CClimbDownStartCheck>(Name);
            break;
        case DDConditionNode::ClimbUpContinueCheck:
            NewTreeNode = m_StateComponent->CreateTreeNode<CClimbUpContinueCheck>(Name);
            break;
        case DDConditionNode::ClimbUpStartCheck:
            NewTreeNode = m_StateComponent->CreateTreeNode<CClimbingUpStartCheck>(Name);
            break;
        case DDConditionNode::ClimbUpEndCheck:
            NewTreeNode = m_StateComponent->CreateTreeNode<CClimbUpEndCheck>(Name);
            break;
        case DDConditionNode::ClimbKeyEnableCheck:
            NewTreeNode = m_StateComponent->CreateTreeNode<CClimbKeyEnableCheck>(Name);
            break;


        case DDConditionNode::Lockstone3TriggerBoxHitCheck:
            NewTreeNode = m_StateComponent->CreateTreeNode<CLockstone3TriggerBoxHitCheck>(Name);
            break;
        case DDConditionNode::CheckDetectRange:
            NewTreeNode = m_StateComponent->CreateTreeNode<CCheckDetectRangeNode>(Name);
            break;
        case DDConditionNode::PlayerEnterZoneCheck:
            NewTreeNode = m_StateComponent->CreateTreeNode<CPlayerEnterZoneCheckNode>(Name);
            break;
        case DDConditionNode::PlayerEnterTriggerIsEnable:
            NewTreeNode = m_StateComponent->CreateTreeNode<CPlayerEnterTriggerIsEnable>(Name);
            break;
        case DDConditionNode::StraightPathCheck:
            NewTreeNode = m_StateComponent->CreateTreeNode<CStraightPathCheck>(Name);
            break;
        case DDConditionNode::MeleeAttackRangeCheck:
            NewTreeNode = m_StateComponent->CreateTreeNode<CMeleeRangeCheckNode>(Name);
            break;
        case DDConditionNode::PathFindExecuteCheck:
            NewTreeNode = m_StateComponent->CreateTreeNode<CPathFindExecuteCheck>(Name);
            break;
        case DDConditionNode::PathFindEnableCheck:
            NewTreeNode = m_StateComponent->CreateTreeNode<CPathFindEnableCheck>(Name);
            break;

        case DDConditionNode::PathListEmptyCheck:
            NewTreeNode = m_StateComponent->CreateTreeNode<CPathListEmptyCheck>(Name);
            break;
        case DDConditionNode::PostAttackDelayCheck:
            NewTreeNode = m_StateComponent->CreateTreeNode<CPostAttackDelayCheck>(Name);
            break;
        case DDConditionNode::AttackCoolTimeCheck:
            NewTreeNode = m_StateComponent->CreateTreeNode<CAttackCoolTimeCheck>(Name);
            break;
        case DDConditionNode::IsCombatCheck:
            NewTreeNode = m_StateComponent->CreateTreeNode<CIsCombatCheck>(Name);
            break;
        case DDConditionNode::DeathCheck:
            NewTreeNode = m_StateComponent->CreateTreeNode<CDeathCheck>(Name);
            break;
        case DDConditionNode::HitCheckNode:
            NewTreeNode = m_StateComponent->CreateTreeNode<CHitCheckNode>(Name);
            break;
        case DDConditionNode::RollInputCheck:
            NewTreeNode = m_StateComponent->CreateTreeNode<CRollInputCheck>(Name);
            break;
        case DDConditionNode::UpdateInputQueue:
            NewTreeNode = m_StateComponent->CreateTreeNode<CUpdateInputQueue>(Name);
            break;


        // Boss Knight
        case DDConditionNode::BossKnightFinalAttackCheck:
            NewTreeNode = m_StateComponent->CreateTreeNode<CBossKnightFinalAttackCheck>(Name);
            break;
        case DDConditionNode::BossKnightJumpAttackRangeCheck:
            NewTreeNode = m_StateComponent->CreateTreeNode<CBossKnightJumpAttackRangeCheck>(Name);
            break;
        case DDConditionNode::BossKnightCutScenePlayCheck:
            NewTreeNode = m_StateComponent->CreateTreeNode<CBossKnightCutScenePlayCheck>(Name);
            break;
        case DDConditionNode::BossKnightPlayerEnterZoneCheck:
            NewTreeNode = m_StateComponent->CreateTreeNode<CBossKnightPlayerEnterZoneCheck>(Name);
            break;
        case DDConditionNode::BossKnightContinueAttackCheck:
            NewTreeNode = m_StateComponent->CreateTreeNode<CBossKnightContinueAttackCheck>(Name);
            break;

        // BossBetty
        case DDConditionNode::BossBettyCheckAttackDir:
            NewTreeNode = m_StateComponent->CreateTreeNode<CBossBettyChangeDirCheck>(Name);
            break;
        case DDConditionNode::BossBettyCheckFarAttackType:
            NewTreeNode = m_StateComponent->CreateTreeNode<CBossBettyFarAttackTypeCheck>(Name);
            break;
        case DDConditionNode::BossBettyCheckHPState:
            NewTreeNode = m_StateComponent->CreateTreeNode<CBossBettyHPStateCheck>(Name);
            break;
        case DDConditionNode::BossBettyCheckThrowAttack:
            NewTreeNode = m_StateComponent->CreateTreeNode<CBossBettyThrowAttackCheck>(Name);
            break;
        case DDConditionNode::BossBettyCheckIdle:
            NewTreeNode = m_StateComponent->CreateTreeNode<CBossBettyIdleAnimCheck>(Name);
            break;
        case DDConditionNode::BossBettyCheckIntro:
            NewTreeNode = m_StateComponent->CreateTreeNode<CBossBettyIntroAnimCheck>(Name);
            break;

        // Crow Boss
        case DDConditionNode::CrowBossCutScenePlayCheck:
            NewTreeNode = m_StateComponent->CreateTreeNode<CCrowBossCutScenePlayCheck>(Name);
            break;
        case DDConditionNode::CrowBossPhasePickNode:
            NewTreeNode = m_StateComponent->CreateTreeNode<CCrowBossPhasePickNode>(Name);
            break;
        case DDConditionNode::CrowBossBypassCheck:
            NewTreeNode = m_StateComponent->CreateTreeNode<CCrowBossBypassCheck>(Name);
            break;
        case DDConditionNode::CrowBossJumpStartCheck:
            NewTreeNode = m_StateComponent->CreateTreeNode<CCrowBossJumpStartCheck>(Name);
            break;
        case DDConditionNode::CrowBossLandingCheck:
            NewTreeNode = m_StateComponent->CreateTreeNode<CCrowBossLandingCheck>(Name);
            break;
        case DDConditionNode::CrowBossJumpAttackRangeCheck:
            NewTreeNode = m_StateComponent->CreateTreeNode<CCrowBossJumpAttackRangeCheck>(Name);
            break;
        case DDConditionNode::CrowBossDoneCheck:
            NewTreeNode = m_StateComponent->CreateTreeNode<CCrowBossDoneCheck>(Name);
            break;
        case DDConditionNode::CrowBossFirstShootCheck:
            NewTreeNode = m_StateComponent->CreateTreeNode<CCrowBossFirstShootCheck>(Name);
            break;
        case DDConditionNode::CrowBossShootCheck:
            NewTreeNode = m_StateComponent->CreateTreeNode<CCrowBossShootCheck>(Name);
            break;
        case DDConditionNode::CrowBossShootEndCheck:
            NewTreeNode = m_StateComponent->CreateTreeNode<CCrowBossShootEndCheck>(Name);
            break;
        case DDConditionNode::CrowBossShootReadyCheck:
            NewTreeNode = m_StateComponent->CreateTreeNode<CCrowBossShootReadyCheck>(Name);
            break;

		// HeadRoller
        case DDConditionNode::HeadRollerStunCheck:
            NewTreeNode = m_StateComponent->CreateTreeNode<CHeadRollerStunCheck>(Name);
            break;

        // Dodger
        case DDConditionNode::DodgerDashCheck:
            NewTreeNode = m_StateComponent->CreateTreeNode<CDodgerDashCheck>(Name);
            break;
        case DDConditionNode::DodgerSecondAttackCheck:
            NewTreeNode = m_StateComponent->CreateTreeNode<CDodgerSecondAttackCheck>(Name);
            break;

        // Bat
        case DDConditionNode::BatRecognizeStartCheck:
            NewTreeNode = m_StateComponent->CreateTreeNode<CBatRecognizeStartCheck>(Name);
            break;
        case DDConditionNode::BatRecognizeEndCheck:
            NewTreeNode = m_StateComponent->CreateTreeNode<CBatRecognizeEndCheck>(Name);
            break;

        // Lurker
        case DDConditionNode::LurkerHopStartCheck:
            NewTreeNode = m_StateComponent->CreateTreeNode<CLurkerHopStartCheck>(Name);
            break;

        // Grunt Common
        case DDConditionNode::GruntCommonSpawnCheck:
            NewTreeNode = m_StateComponent->CreateTreeNode<CGruntCommonSpawnCheckNode>(Name);
            break;
        case DDConditionNode::GruntCommonStartActCheck:
            NewTreeNode = m_StateComponent->CreateTreeNode<CGruntCommonMoveStartCheckNode>(Name);
            break;
        }
        break;
    }

    case 4:
    {
        enum DecoratorNode NodeDecoratorClass;
        NodeDecoratorClass = static_cast<DecoratorNode>(0);
        //NodeDecoratorClass = static_cast<DecoratorNode>(ActionIndex);

        // TODO : Decorator Node 종류 추가될 때 마다 추가
        switch (NodeDecoratorClass)
        {
        case DecoratorNode::Negate:
            NewTreeNode = m_StateComponent->CreateTreeNode<CNegateNode>(NewName);
            break;
        }
    }
    }

    NewTreeNode->SetOwner(m_StateComponent->GetBehaviorTree());
    NewTreeNode->SetObject(m_StateComponent->GetGameObject());
    NewTreeNode->SetAnimationMeshComponent(m_StateComponent->GetAnimationMeshComp());

    m_Delegate.AddNode(NodeName, idx, 0.f, 0.f, false, NewTreeNode);

    if (!m_StateComponent->GetBehaviorTree()->GetRootNode())
    {
        m_StateComponent->GetBehaviorTree()->SetRoot(NewTreeNode);
    }

    m_PrevViewName = "Select Node Type";
    m_LeafNodePrevViewName = "Select Node Action";
}

void CBehaviorTreeWindow::OnDeleteNodeButton(const char* Name)
{
    CBehaviorTree* Tree = m_StateComponent->GetBehaviorTree();

    if (Tree->GetRootNode())
    {
        if (Name == Tree->GetRootNode()->GetName())
        {
            Tree->SetRoot(nullptr);
        }
    }

	CNode* Node = Tree->FindNode(Name);

	GraphEditorDelegate::Node node = m_Delegate.FindNode(Node->GetName());

	int NodeIdx = node.NodeIndex;

	// GUI상에서도 Node랑 (연결된 Link가 있다면)Link 지우기
	m_Delegate.DelAllLink(NodeIdx);
	m_Delegate.DeleteNode(NodeIdx);

	// Node가 지워지면 Node의 NodeIndex랑 Link에 적힌 Input/Output Node Index도 바뀌어야한다
	size_t NodeCount = m_Delegate.mNodes.size();

	for (int i = 0; i < NodeCount; ++i)
	{
		if (m_Delegate.mNodes[i].NodeIndex > NodeIdx)
			--m_Delegate.mNodes[i].NodeIndex;
	}


	size_t Count = m_Delegate.mLinks.size();
	for (int i = 0; i < Count; ++i)
	{
		if (m_Delegate.mLinks[i].mInputNodeIndex > NodeIdx)
			--m_Delegate.mLinks[i].mInputNodeIndex;

		if (m_Delegate.mLinks[i].mOutputNodeIndex > NodeIdx)
			--m_Delegate.mLinks[i].mOutputNodeIndex;
	}

	// Engine상에서도 지우기
	if (Node)
	{
		Tree->DeleteNode(Node);
	}

}


void CBehaviorTreeWindow::PrintActionNodes()
{
    std::string TypeStr;

    for (int i = 0; i < (int)ActionNode::ActionNodeMax; ++i)
    {
        TypeStr = CEditorUtil::ActioNodeTypeToString((ActionNode)i);
        m_vecNodeAction.push_back(TypeStr);
    }

    for (int i = 0; i < (int)DDActionNode::Max; ++i)
    {
        TypeStr = CDDUtil::DDActionNodeTypeToString((DDActionNode)i);
        m_vecNodeAction.push_back(TypeStr);
    }
}

void CBehaviorTreeWindow::PrintConditionNodes()
{
    std::string TypeStr;

    for (int i = 0; i < (int)ConditionNode::ConditionNodeMax; ++i)
    {
        TypeStr = CEditorUtil::ConditionNodeTypeToString((ConditionNode)i);
        m_vecNodeAction.push_back(TypeStr);
    }

    for (int i = 0; i < (int)DDConditionNode::Max; ++i)
    {
        TypeStr = CDDUtil::DDConditionNodeTypeToString((DDConditionNode)i);
        m_vecNodeAction.push_back(TypeStr);
    }
}

void CBehaviorTreeWindow::UpdateLoadNode(CCompositeNode* RootNode)
{
    if (!RootNode)
        return;

    int TypeIndex = -1;

    if (RootNode->GetTypeID() == typeid(CSequenceNode).hash_code())
    {
        TypeIndex = 0;
    }

    else if (RootNode->GetTypeID() == typeid(CSelectorNode).hash_code())
    {
        TypeIndex = 1;
    }

    else if (RootNode->GetNodeType() == Node_Type::Action)
    {
        TypeIndex = 2;
    }

    else if (RootNode->GetNodeType() == Node_Type::Condition)
    {
        TypeIndex = 3;
    }

    Vector2 Pos = RootNode->GetInWindowPos();

    m_Delegate.AddNode(RootNode->GetName(), TypeIndex, Pos.x, Pos.y, false, RootNode);
    RootNode->SetOwner(m_StateComponent->GetBehaviorTree());
    RootNode->SetObject(m_StateComponent->GetGameObject());

    const std::list<CNode*> NodeList = RootNode->GetChildrenList();
    auto iter = NodeList.begin();
    auto iterEnd = NodeList.end();

    for (; iter != iterEnd; ++iter)
    {
        (*iter)->SetOwner(m_StateComponent->GetBehaviorTree());
        (*iter)->SetObject(m_StateComponent->GetGameObject());
        UpdateLoadNodeRecursive(*iter);
    }

    UpdateLoadNodeLink(RootNode->GetOwner());
}

void CBehaviorTreeWindow::UpdateLoadNodeRecursive(CNode* Node)
{
    int TypeIndex = -1;
    std::list<CNode*> NodeList;

    if (Node->GetTypeID() == typeid(CSequenceNode).hash_code())
    {
        TypeIndex = 0;
        NodeList = ((CSequenceNode*)Node)->GetChildrenList();

        auto iter = NodeList.begin();
        auto iterEnd = NodeList.end();

        for (; iter != iterEnd; ++iter)
        {
            UpdateLoadNodeRecursive(*iter);
        }

        Vector2 Pos = Node->GetInWindowPos();

        m_Delegate.AddNode(Node->GetName(), TypeIndex, Pos.x, Pos.y, false, Node);
    }

    else if (Node->GetTypeID() == typeid(CSelectorNode).hash_code())
    {
        TypeIndex = 1;
        NodeList = ((CSelectorNode*)Node)->GetChildrenList();

        auto iter = NodeList.begin();
        auto iterEnd = NodeList.end();


        for (; iter != iterEnd; ++iter)
        {
            UpdateLoadNodeRecursive(*iter);
        }

        Vector2 Pos = Node->GetInWindowPos();

        m_Delegate.AddNode(Node->GetName(), TypeIndex, Pos.x, Pos.y, false, Node);
    }

    else if (Node->GetNodeType() == Node_Type::Action)
    {
        TypeIndex = 2;
        Vector2 Pos = Node->GetInWindowPos();

        m_Delegate.AddNode(Node->GetName(), TypeIndex, Pos.x, Pos.y, false, Node);
    }

    else if (Node->GetNodeType() == Node_Type::Condition)
    {
        TypeIndex = 3;
        Vector2 Pos = Node->GetInWindowPos();

        m_Delegate.AddNode(Node->GetName(), TypeIndex, Pos.x, Pos.y, false, Node);
    }

    else if (Node->GetNodeType() == Node_Type::Decorator)
    {
        TypeIndex = 4;

        UpdateLoadNodeRecursive(((CDecoratorNode*)Node)->GetChild());

        Vector2 Pos = Node->GetInWindowPos();

        m_Delegate.AddNode(Node->GetName(), TypeIndex, Pos.x, Pos.y, false, Node);
    }
}

void CBehaviorTreeWindow::Clear()
{

    m_Delegate.mLinks.clear();

    size_t Count = m_Delegate.mNodes.size();

    for (size_t i = 0; i < Count; ++i)
    {
        SAFE_DELETE_ARRAY(m_Delegate.mNodes[i].name);
    }

    m_Delegate.mNodes.clear();
}

void CBehaviorTreeWindow::UpdateLoadNodeLink(CBehaviorTree* Tree)
{
    size_t Count = Tree->GetNodeCount();
    size_t DelegateNodeCount = m_Delegate.GetNodeCount();
    size_t LinkCount = m_Delegate.mLinks.size();

    for (size_t i = 0; i < Count; ++i)
    {
        CNode* Node = Tree->GetNode((int)i);

        if (Node->GetTypeID() == typeid(CSelectorNode).hash_code() || Node->GetTypeID() == typeid(CSequenceNode).hash_code())
        {

            for (size_t j = 0; j < DelegateNodeCount; ++j)
            {
                if (m_Delegate.GetDelegateNode((int)j).BehaviorTreeNode == Node)
                {
                    GraphEditorDelegate::Node SrcNode = m_Delegate.GetDelegateNode((int)j);

                    const std::list<CNode*> ChildrenList = ((CCompositeNode*)Node)->GetChildrenList();

                    auto iter = ChildrenList.begin();
                    auto iterEnd = ChildrenList.end();

                    int ChildIdx = 0;
                    for (; iter != iterEnd; ++iter, ++ChildIdx)
                    {
                        //GraphEditorDelegate::Node DestNode = m_Delegate.FindNode((*iter)->GetName());
                        GraphEditorDelegate::Node DestNode = m_Delegate.FindNode((*iter));
                        m_Delegate.LoadLink(SrcNode.NodeIndex, ChildIdx, DestNode.NodeIndex, 0);
                    }
                }
            }
        }


        else if (Node->GetNodeType() == Node_Type::Decorator)
        {
            for (size_t j = 0; j < DelegateNodeCount; ++j)
            {
                if (m_Delegate.GetDelegateNode((int)j).BehaviorTreeNode == Node)
                {
                    GraphEditorDelegate::Node SrcNode = m_Delegate.GetDelegateNode((int)j);

                    CNode* DecoratorChild = ((CDecoratorNode*)Node)->GetChild();
                    GraphEditorDelegate::Node DestNode = m_Delegate.FindNode(DecoratorChild);

                    m_Delegate.LoadLink(SrcNode.NodeIndex, 0, DestNode.NodeIndex, 0);
                }

            }
        }
    }
}

