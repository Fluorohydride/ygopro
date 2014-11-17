--神騎セイントレア
function c36776089.initial_effect(c)
    --xyz summon
    aux.AddXyzProcedure(c,aux.XyzFilterFunction(c,2),2)
    c:EnableReviveLimit()
    --battle indestructable
    local e1=Effect.CreateEffect(c)
    e1:SetType(EFFECT_TYPE_SINGLE)
    e1:SetCode(EFFECT_INDESTRUCTABLE_BATTLE)
    e1:SetCondition(c36776089.con)
    e1:SetValue(1)
    c:RegisterEffect(e1)
    --to hand
    local e2=Effect.CreateEffect(c)
    e2:SetDescription(aux.Stringid(36776089,0))
    e2:SetCategory(CATEGORY_TOHAND)
    e2:SetType(EFFECT_TYPE_SINGLE+EFFECT_TYPE_TRIGGER_O)
    e2:SetCode(EVENT_DAMAGE_STEP_END)
    e2:SetCost(c36776089.thcost)
    e2:SetTarget(c36776089.thtg)
    e2:SetOperation(c36776089.thop)
    c:RegisterEffect(e2)
end
function c36776089.con(e)
    return e:GetHandler():GetOverlayCount()>0
end
function c36776089.thcost(e,tp,eg,ep,ev,re,r,rp,chk)
    if chk==0 then return e:GetHandler():CheckRemoveOverlayCard(tp,1,REASON_COST) end
    e:GetHandler():RemoveOverlayCard(tp,1,1,REASON_COST)
end
function c36776089.thtg(e,tp,eg,ep,ev,re,r,rp,chk)
    local tc=e:GetHandler():GetBattleTarget()
    if chk==0 then return tc and tc:IsRelateToBattle() and tc:IsAbleToHand() end
    Duel.SetOperationInfo(0,CATEGORY_TOHAND,nil,1,0,0)
end
function c36776089.thop(e,tp,eg,ep,ev,re,r,rp)
    local tc=e:GetHandler():GetBattleTarget()
    if tc:IsRelateToBattle() and tc:IsAbleToHand() then
        Duel.SendtoHand(tc,nil,REASON_EFFECT)
    end
end