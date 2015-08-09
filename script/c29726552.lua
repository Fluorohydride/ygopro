--Kumongous the Sticky Thread Kaiju
function c29726552.initial_effect(c)
    c:SetUniqueOnField(1,0,0x223)
    --special summon
    local e1=Effect.CreateEffect(c)
    e1:SetType(EFFECT_TYPE_FIELD)
    e1:SetCode(EFFECT_SPSUMMON_PROC)
    e1:SetProperty(EFFECT_FLAG_UNCOPYABLE+EFFECT_FLAG_SPSUM_PARAM)
    e1:SetRange(LOCATION_HAND)
    e1:SetTargetRange(POS_FACEUP_ATTACK,1)
    e1:SetCondition(c29726552.spcon)
    e1:SetOperation(c29726552.spop)
    c:RegisterEffect(e1)
    local e2=Effect.CreateEffect(c)
    e2:SetType(EFFECT_TYPE_FIELD)
    e2:SetCode(EFFECT_SPSUMMON_PROC)
    e2:SetProperty(EFFECT_FLAG_UNCOPYABLE+EFFECT_FLAG_SPSUM_PARAM)
    e2:SetRange(LOCATION_HAND)
    e2:SetTargetRange(POS_FACEUP_ATTACK,0)
    e2:SetCondition(c29726552.spcon2)
    c:RegisterEffect(e2)
    --atkdown
    local e3=Effect.CreateEffect(c)
    e3:SetCategory(CATEGORY_ATKCHANGE)
    e3:SetType(EFFECT_TYPE_FIELD+EFFECT_TYPE_TRIGGER_O)
    e3:SetCode(EVENT_SPSUMMON_SUCCESS)
    e3:SetRange(LOCATION_MZONE)
    e3:SetCost(c29726552.cost)
    e3:SetTarget(c29726552.target)
    e3:SetOperation(c29726552.operation)
    c:RegisterEffect(e3)
    local e4=e3:Clone()
    e4:SetCode(EVENT_SUMMON_SUCCESS)
    c:RegisterEffect(e4)
end
function c29726552.spcon(e,c)
    if c==nil then return true end
    local tp=c:GetControler()
    return Duel.GetLocationCount(1-tp,LOCATION_MZONE)>-1
        and Duel.IsExistingMatchingCard(Card.IsReleasable,tp,0,LOCATION_MZONE,1,nil)
        and not Duel.IsExistingMatchingCard(c29726552.spfilter,c:GetControler(),0,LOCATION_MZONE,1,nil)
end
function c29726552.spop(e,tp,eg,ep,ev,re,r,rp,c)
    Duel.Hint(HINT_SELECTMSG,tp,HINTMSG_RELEASE)
    local g=Duel.SelectMatchingCard(tp,Card.IsReleasable,tp,0,LOCATION_MZONE,1,1,nil)
    Duel.Release(g,REASON_COST)
end

function c29726552.spfilter(c)
    return c:IsFaceup() and c:IsSetCard(0x223)
end
function c29726552.spcon2(e,c)
    if c==nil then return true end
    return Duel.GetLocationCount(c:GetControler(),LOCATION_MZONE)>0 and
        Duel.IsExistingMatchingCard(c29726552.spfilter,c:GetControler(),0,LOCATION_MZONE,1,nil)
end

function c29726552.filter(c,e,tp)
    return c:IsFaceup() and c:IsControler(1-tp) and (not e or c:IsRelateToEffect(e))
end
function c29726552.cost(e,tp,eg,ep,ev,re,r,rp,chk)
    if chk==0 then return Duel.IsCanRemoveCounter(tp,1,1,0x2a,2,REASON_COST) end
    Duel.RemoveCounter(tp,1,1,0x2a,2,REASON_COST)
end
function c29726552.target(e,tp,eg,ep,ev,re,r,rp,chk)
    if chk==0 then return eg:IsExists(c29726552.filter,1,nil,nil,tp) end
    Duel.SetTargetCard(eg)
end
function c29726552.operation(e,tp,eg,ep,ev,re,r,rp)
    local c=e:GetHandler()
    local g=eg:Filter(c29726552.filter,nil,e,tp)
    local tc=g:GetFirst()
    while tc do
        local e1=Effect.CreateEffect(c)
        e1:SetType(EFFECT_TYPE_SINGLE)
        e1:SetCode(EFFECT_CANNOT_ATTACK)
        e1:SetReset(RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_END,2)
        tc:RegisterEffect(e1)
        local e2=Effect.CreateEffect(c)
        e2:SetType(EFFECT_TYPE_SINGLE)
        e2:SetCode(EFFECT_DISABLE)
        e2:SetReset(RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_END,2)
        tc:RegisterEffect(e2)
        local e3=Effect.CreateEffect(c)
        e3:SetType(EFFECT_TYPE_SINGLE)
        e3:SetCode(EFFECT_DISABLE_EFFECT)
        e3:SetValue(RESET_TURN_SET)
        e3:SetReset(RESET_EVENT+0x1fe0000+RESET_PHASE+PHASE_END,2)
        tc:RegisterEffect(e3)
        tc=g:GetNext()
    end
end
