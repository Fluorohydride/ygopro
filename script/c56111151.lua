--Kyoutou Seafront
function c56111151.initial_effect(c)
    c:EnableCounterPermit(0x2a)
    c:SetCounterLimit(0x2a,5)
    --Activate
    local e1=Effect.CreateEffect(c)
    e1:SetType(EFFECT_TYPE_ACTIVATE)
    e1:SetCode(EVENT_FREE_CHAIN)
    c:RegisterEffect(e1)
    --damage
    local e2=Effect.CreateEffect(c)
    e2:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_CONTINUOUS)
    e2:SetCode(EVENT_TO_GRAVE)
    e2:SetRange(LOCATION_SZONE)
    e2:SetOperation(c56111151.counter)
    c:RegisterEffect(e2)
    local e3=Effect.CreateEffect(c)
    e3:SetType(EFFECT_TYPE_IGNITION)
    e3:SetRange(LOCATION_SZONE)
    e3:SetCountLimit(1)
    e3:SetTarget(c56111151.target)
    e3:SetOperation(c56111151.operation)
    c:RegisterEffect(e3)
    --Destroy replace
    local e4=Effect.CreateEffect(c)
    e4:SetType(EFFECT_TYPE_CONTINUOUS+EFFECT_TYPE_SINGLE)
    e4:SetCode(EFFECT_DESTROY_REPLACE)
    e4:SetProperty(EFFECT_FLAG_SINGLE_RANGE)
    e4:SetRange(LOCATION_SZONE)
    e4:SetTarget(c56111151.desreptg)
    e4:SetOperation(c56111151.desrepop)
    c:RegisterEffect(e4)
end
function c56111151.filter(c,tp)
    return c:IsPreviousLocation(LOCATION_ONFIELD)
end
function c56111151.counter(e,tp,eg,ep,ev,re,r,rp)
    local ct=eg:FilterCount(c56111151.filter,nil,tp)
    if ct>0 then
        e:GetHandler():AddCounter(0x2a,1)
    end
end
function c56111151.kfilter(c)
    return c:IsSetCard(0x223) and c:IsType(TYPE_MONSTER) and c:IsAbleToHand()
end
function c56111151.target(e,tp,eg,ep,ev,re,r,rp,chk)
    if chk==0 then return Duel.IsExistingMatchingCard(c56111151.kfilter,tp,LOCATION_DECK,0,1,nil) 
    and e:GetHandler():IsCanRemoveCounter(tp,0x2a,3,REASON_COST) end
    Duel.SetOperationInfo(0,CATEGORY_TOHAND,nil,1,tp,LOCATION_DECK)
end
function c56111151.operation(e,tp,eg,ep,ev,re,r,rp)
    Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_ATOHAND)
    local g=Duel.SelectMatchingCard(tp,c56111151.kfilter,tp,LOCATION_DECK,0,1,1,nil)
    if g:GetCount()>0 then
        Duel.SendtoHand(g,nil,REASON_EFFECT)
        Duel.ConfirmCards(1-tp,g)
    end
end

function c56111151.desreptg(e,tp,eg,ep,ev,re,r,rp,chk)
    if chk==0 then return not e:GetHandler():IsReason(REASON_RULE)
        and e:GetHandler():GetCounter(0x2a)>0 end
    return Duel.SelectYesNo(tp,aux.Stringid(56111151,0))
end
function c56111151.desrepop(e,tp,eg,ep,ev,re,r,rp)
    e:GetHandler():RemoveCounter(ep,0x2a,1,REASON_EFFECT)
end