--Scheduled Extinction
function c95676943.initial_effect(c)
    --Activate
    local e1=Effect.CreateEffect(c)
    e1:SetCategory(CATEGORY_DESTROY)
    e1:SetType(EFFECT_TYPE_ACTIVATE)
    e1:SetCode(EVENT_FREE_CHAIN)
    e1:SetHintTiming(TIMING_MAIN_END,TIMING_MAIN_END)
    e1:SetCost(c95676943.cost)
    e1:SetCondition(c95676943.condition)
    e1:SetOperation(c95676943.nuke)
    c:RegisterEffect(e1)
end
function c95676943.condition(e,tp,eg,ep,ev,re,r,rp)
    return (Duel.GetCurrentPhase()==PHASE_MAIN1 or Duel.GetCurrentPhase()==PHASE_MAIN2)
end
function c95676943.cost(e,tp,eg,ep,ev,re,r,rp,chk)
    if chk==0 then return true end
    Duel.PayLPCost(tp,2000)
end
function c95676943.nuke(e,tp,eg,ep,ev,re,r,rp)
    local c=e:GetHandler()
        local e1=Effect.CreateEffect(e:GetHandler())
        e1:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
        e1:SetCode(EVENT_PHASE+PHASE_BATTLE)
        e1:SetCountLimit(1)
        e1:SetReset(RESET_PHASE+PHASE_BATTLE,3)
        e1:SetOperation(c95676943.activate)
        e1:SetLabel(1)
        Duel.RegisterEffect(e1,tp)
end
function c95676943.activate(e,tp,eg,ep,ev,re,r,rp)
    local ct=e:GetLabel()
    e:GetOwner():SetTurnCounter(ct)
    e:SetLabel(ct+1)
    if ct==3 then
        local g=Duel.GetMatchingGroup(Card.IsDestructable,tp,LOCATION_ONFIELD,LOCATION_ONFIELD,e:GetHandler())
        Duel.Destroy(g,REASON_EFFECT)
    end
end

